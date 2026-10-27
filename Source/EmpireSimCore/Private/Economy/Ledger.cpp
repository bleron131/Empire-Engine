// Empire Engine - engine-independent simulation core.
#include "Empire/Economy/Ledger.h"

#include <optional>
#include <unordered_set>
#include <utility>

namespace empire
{

const char* LexToString(ELedgerError Error)
{
	switch (Error)
	{
	case ELedgerError::None: return "None";
	case ELedgerError::MissingIdempotencyKey: return "MissingIdempotencyKey";
	case ELedgerError::InvalidAmount: return "InvalidAmount";
	case ELedgerError::IdempotencyConflict: return "IdempotencyConflict";
	case ELedgerError::TooFewPostings: return "TooFewPostings";
	case ELedgerError::ZeroPosting: return "ZeroPosting";
	case ELedgerError::DuplicateAccount: return "DuplicateAccount";
	case ELedgerError::UnknownAccount: return "UnknownAccount";
	case ELedgerError::Unbalanced: return "Unbalanced";
	case ELedgerError::InsufficientFunds: return "InsufficientFunds";
	case ELedgerError::Overflow: return "Overflow";
	}
	return "Unknown";
}

FAccountId FLedger::OpenAccount(const FAccountSpec& Spec)
{
	FAccount Account;
	Account.Id = FAccountId{NextAccountId++};
	Account.DisplayName = Spec.DisplayName;
	Account.Kind = Spec.Kind;
	Account.Owner = Spec.Owner;
	Account.MinimumBalance = Spec.MinimumBalance;

	const FAccountId Id = Account.Id;
	Accounts.emplace(Id, std::move(Account));
	return Id;
}

const FAccount* FLedger::FindAccount(FAccountId Id) const
{
	const auto It = Accounts.find(Id);
	return It != Accounts.end() ? &It->second : nullptr;
}

FTransactionResult FLedger::Commit(const FTransactionRequest& Request)
{
	FTransactionResult Result;

	if (Request.IdempotencyKey.empty())
	{
		Result.Error = ELedgerError::MissingIdempotencyKey;
		return Result;
	}

	if (const auto Existing = JournalIndexByKey.find(Request.IdempotencyKey); Existing != JournalIndexByKey.end())
	{
		const FTransactionRecord& Record = Journal[Existing->second];
		if (Record.Postings == Request.Postings && Record.Reason == Request.Reason)
		{
			Result.Id = Record.Id;
			Result.bDuplicate = true;
		}
		else
		{
			Result.Error = ELedgerError::IdempotencyConflict;
		}
		return Result;
	}

	if (Request.Postings.size() < 2)
	{
		Result.Error = ELedgerError::TooFewPostings;
		return Result;
	}

	// Phase 1: validate everything and compute the resulting balances without touching state.
	std::vector<std::pair<FAccount*, FMoney>> NewBalances;
	NewBalances.reserve(Request.Postings.size());
	std::unordered_set<FAccountId> SeenAccounts;
	FMoney Sum = FMoney::Zero();

	for (const FPosting& Posting : Request.Postings)
	{
		Result.OffendingAccount = Posting.Account;

		if (Posting.Delta.IsZero())
		{
			Result.Error = ELedgerError::ZeroPosting;
			return Result;
		}
		if (!SeenAccounts.insert(Posting.Account).second)
		{
			Result.Error = ELedgerError::DuplicateAccount;
			return Result;
		}

		const auto It = Accounts.find(Posting.Account);
		if (It == Accounts.end())
		{
			Result.Error = ELedgerError::UnknownAccount;
			return Result;
		}
		FAccount& Account = It->second;

		const std::optional<FMoney> NewSum = FMoney::CheckedAdd(Sum, Posting.Delta);
		const std::optional<FMoney> NewBalance = FMoney::CheckedAdd(Account.Balance, Posting.Delta);
		if (!NewSum || !NewBalance)
		{
			Result.Error = ELedgerError::Overflow;
			return Result;
		}
		Sum = *NewSum;

		// Only debits are limited: a credit must always be accepted, even if the account
		// is already below its minimum (e.g. after a credit line was reduced).
		if (Account.Kind != EAccountKind::System && Posting.Delta.IsNegative() && *NewBalance < Account.MinimumBalance)
		{
			Result.Error = ELedgerError::InsufficientFunds;
			return Result;
		}

		NewBalances.emplace_back(&Account, *NewBalance);
	}

	Result.OffendingAccount = FAccountId{};
	if (!Sum.IsZero())
	{
		Result.Error = ELedgerError::Unbalanced;
		return Result;
	}

	// Phase 2: apply. Nothing below can fail, which makes the commit atomic.
	for (const auto& [Account, NewBalance] : NewBalances)
	{
		Account->Balance = NewBalance;
	}

	FTransactionRecord Record;
	Record.Id = FTransactionId{NextTransactionId++};
	Record.IdempotencyKey = Request.IdempotencyKey;
	Record.Reason = Request.Reason;
	Record.GameTime = Request.GameTime;
	Record.Postings = Request.Postings;
	Record.Memo = Request.Memo;

	Result.Id = Record.Id;
	JournalIndexByKey.emplace(Record.IdempotencyKey, Journal.size());
	Journal.push_back(std::move(Record));
	return Result;
}

FTransactionResult FLedger::Transfer(std::string IdempotencyKey, FAccountId From, FAccountId To, FMoney Amount,
	ETransactionReason Reason, std::int64_t GameTime, std::string Memo)
{
	FTransactionRequest Request;
	Request.IdempotencyKey = std::move(IdempotencyKey);
	Request.Reason = Reason;
	Request.GameTime = GameTime;
	Request.Memo = std::move(Memo);

	const std::optional<FMoney> Debit = FMoney::CheckedNegate(Amount);
	if (!Amount.IsPositive() || !Debit)
	{
		FTransactionResult Result;
		Result.Error = ELedgerError::InvalidAmount;
		return Result;
	}

	Request.Postings = {FPosting{From, *Debit}, FPosting{To, Amount}};
	return Commit(Request);
}

std::optional<FMoney> FLedger::GetCirculatingMoney() const
{
	std::optional<FMoney> Total = FMoney::Zero();
	for (const auto& [Id, Account] : Accounts)
	{
		if (Account.Kind != EAccountKind::System)
		{
			Total = FMoney::CheckedAdd(*Total, Account.Balance);
			if (!Total)
			{
				return std::nullopt;
			}
		}
	}
	return Total;
}

FLedgerIntegrityReport FLedger::VerifyIntegrity() const
{
	FLedgerIntegrityReport Report;
	auto Fail = [&Report](std::string Problem)
	{
		Report.bOk = false;
		Report.Problems.push_back(std::move(Problem));
	};

	std::unordered_map<FAccountId, FMoney> Replayed;
	for (const auto& [Id, Account] : Accounts)
	{
		Replayed.emplace(Id, FMoney::Zero());
	}

	std::unordered_set<std::string> Keys;
	for (const FTransactionRecord& Record : Journal)
	{
		const std::string Tx = "transaction " + std::to_string(Record.Id.Value);
		if (!Keys.insert(Record.IdempotencyKey).second)
		{
			Fail(Tx + ": duplicate idempotency key '" + Record.IdempotencyKey + "'");
		}

		FMoney Sum = FMoney::Zero();
		for (const FPosting& Posting : Record.Postings)
		{
			const auto It = Replayed.find(Posting.Account);
			if (It == Replayed.end())
			{
				Fail(Tx + ": references unknown account " + std::to_string(Posting.Account.Value));
				continue;
			}
			const std::optional<FMoney> NewBalance = FMoney::CheckedAdd(It->second, Posting.Delta);
			const std::optional<FMoney> NewSum = FMoney::CheckedAdd(Sum, Posting.Delta);
			if (!NewBalance || !NewSum)
			{
				Fail(Tx + ": overflow during replay");
				continue;
			}
			It->second = *NewBalance;
			Sum = *NewSum;
		}
		if (!Sum.IsZero())
		{
			Fail(Tx + ": postings do not sum to zero");
		}
	}

	for (const auto& [Id, Account] : Accounts)
	{
		if (Replayed[Id] != Account.Balance)
		{
			Fail("account " + std::to_string(Id.Value) + ": stored balance " + std::to_string(Account.Balance.Minor()) +
				" != replayed balance " + std::to_string(Replayed[Id].Minor()));
		}
	}

	return Report;
}

} // namespace empire
