// Empire Engine - engine-independent simulation core.
#pragma once

#include "Empire/Core/CoreDefines.h"
#include "Empire/Core/Ids.h"
#include "Empire/Core/Money.h"

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace empire
{

enum class EAccountKind : std::uint8_t
{
	// Personal money of a player or NPC character.
	Private,
	// Money of a company. Kept strictly separate from private accounts.
	Business,
	// Represents the world outside the simulated economy (NPC customers, suppliers,
	// the state). System accounts may go negative: their balances mirror the money
	// that entered or left the economy, which keeps the double-entry sum at zero.
	System,
};

enum class EOwnerType : std::uint8_t
{
	World,
	Player,
	Business,
};

// Who an account belongs to. Ownership is not permission: who may move money
// is decided by the authorization layer above the ledger.
struct FOwnerRef
{
	EOwnerType Type = EOwnerType::World;
	std::uint64_t Id = 0;

	bool operator==(const FOwnerRef&) const = default;
};

struct FAccountSpec
{
	std::string DisplayName;
	EAccountKind Kind = EAccountKind::Private;
	FOwnerRef Owner;
	// Lowest balance the account may reach (0 = no overdraft, negative = credit line).
	// Ignored for system accounts.
	FMoney MinimumBalance = FMoney::Zero();
};

struct FAccount
{
	FAccountId Id;
	std::string DisplayName;
	EAccountKind Kind = EAccountKind::Private;
	FOwnerRef Owner;
	FMoney MinimumBalance = FMoney::Zero();
	FMoney Balance = FMoney::Zero();
};

enum class ETransactionReason : std::uint16_t
{
	Unspecified,
	JobReward,
	Purchase,
	Sale,
	Salary,
	Investment,
	Loan,
	LoanRepayment,
	Dividend,
	InternalTransfer,
	Fee,
	Adjustment,
};

// One signed change to one account. Positive = credit, negative = debit.
struct FPosting
{
	FAccountId Account;
	FMoney Delta;

	bool operator==(const FPosting&) const = default;
};

struct FTransactionRequest
{
	// Required. Identifies one logical economic operation (e.g. "job:4711:payout").
	// Committing the same key twice never pays twice.
	std::string IdempotencyKey;
	ETransactionReason Reason = ETransactionReason::Unspecified;
	// Timestamp from the shared game clock (game seconds since world start).
	std::int64_t GameTime = 0;
	// At least two postings, each account at most once, deltas summing to zero.
	std::vector<FPosting> Postings;
	std::string Memo;
};

struct FTransactionRecord
{
	FTransactionId Id;
	std::string IdempotencyKey;
	ETransactionReason Reason = ETransactionReason::Unspecified;
	std::int64_t GameTime = 0;
	std::vector<FPosting> Postings;
	std::string Memo;
};

enum class ELedgerError : std::uint8_t
{
	None,
	MissingIdempotencyKey,
	// Transfer amount was zero or negative.
	InvalidAmount,
	// Key was already used for a transaction with different content.
	IdempotencyConflict,
	TooFewPostings,
	ZeroPosting,
	DuplicateAccount,
	UnknownAccount,
	Unbalanced,
	InsufficientFunds,
	Overflow,
};

EMPIRESIMCORE_API const char* LexToString(ELedgerError Error);

struct FTransactionResult
{
	ELedgerError Error = ELedgerError::None;
	// Id of the committed transaction (also set when Duplicate is true).
	FTransactionId Id;
	// True if the idempotency key was already committed with identical content;
	// nothing was changed by this call.
	bool bDuplicate = false;
	// Account that caused the rejection, if the error relates to one.
	FAccountId OffendingAccount;

	bool Succeeded() const { return Error == ELedgerError::None; }
};

struct FLedgerIntegrityReport
{
	bool bOk = true;
	std::vector<std::string> Problems;
};

// Double-entry ledger: the single source of truth for all money in a world.
//
// Guarantees:
//  - Every transaction is atomic: it is fully validated before any balance changes,
//    so it is applied completely or not at all.
//  - The sum of all balances is always zero; money can only enter the player
//    economy through system accounts, which makes every unit of money traceable.
//  - Idempotency keys prevent double payouts on retries or duplicated network messages.
//  - Rejected transactions change nothing and do not consume their idempotency key.
//
// Not thread-safe by design: owned and mutated only by the authoritative host.
//
// Members are exported individually instead of the whole class: exporting a class with
// std:: members from a DLL (Unreal's modular editor builds) triggers MSVC warning C4251.
class FLedger
{
public:
	EMPIRESIMCORE_API FAccountId OpenAccount(const FAccountSpec& Spec);

	EMPIRESIMCORE_API const FAccount* FindAccount(FAccountId Id) const;

	EMPIRESIMCORE_API FTransactionResult Commit(const FTransactionRequest& Request);

	// Convenience for the common two-party case: moves Amount (> 0) from one account to another.
	EMPIRESIMCORE_API FTransactionResult Transfer(std::string IdempotencyKey, FAccountId From, FAccountId To, FMoney Amount,
		ETransactionReason Reason, std::int64_t GameTime, std::string Memo = {});

	const std::vector<FTransactionRecord>& GetJournal() const { return Journal; }

	// Sum of all non-system balances: the money currently held by players, NPCs and companies.
	// Internal transfers between participants never change this value.
	// Returns std::nullopt if the total is not representable.
	EMPIRESIMCORE_API std::optional<FMoney> GetCirculatingMoney() const;

	// Replays the journal from zero and compares the result with the stored balances.
	// Used after loading and in tests to detect corrupted or inconsistent state.
	EMPIRESIMCORE_API FLedgerIntegrityReport VerifyIntegrity() const;

private:
	std::unordered_map<FAccountId, FAccount> Accounts;
	std::vector<FTransactionRecord> Journal;
	std::unordered_map<std::string, std::size_t> JournalIndexByKey;
	std::uint64_t NextAccountId = 1;
	std::uint64_t NextTransactionId = 1;
};

} // namespace empire
