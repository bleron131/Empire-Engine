// Empire Engine - tests for FLedger (atomicity, idempotency, money conservation).
#include "Framework/TestFramework.h"

#include "Empire/Economy/Ledger.h"

#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace empire;

namespace
{

FMoney M(FMoney::Rep Minor)
{
	return FMoney::FromMinor(Minor);
}

struct FFixture
{
	FLedger Ledger;
	FAccountId World = Ledger.OpenAccount({"World", EAccountKind::System, {EOwnerType::World, 0}});
	FAccountId Alice = Ledger.OpenAccount({"Alice", EAccountKind::Private, {EOwnerType::Player, 1}});
	FAccountId Bob = Ledger.OpenAccount({"Bob", EAccountKind::Private, {EOwnerType::Player, 2}});
	FAccountId AliceCo = Ledger.OpenAccount({"Alice Transport", EAccountKind::Business, {EOwnerType::Business, 10}});

	FMoney Balance(FAccountId Id) const { return Ledger.FindAccount(Id)->Balance; }

	void Fund(FAccountId Id, FMoney::Rep Minor, const std::string& Key)
	{
		Ledger.Transfer(Key, World, Id, M(Minor), ETransactionReason::Adjustment, 0);
	}
};

} // namespace

EMPIRE_TEST(Ledger_OpenAccountAssignsUniqueValidIds)
{
	FFixture F;
	CHECK(F.World.IsValid());
	CHECK(F.Alice != F.Bob);
	CHECK(F.Ledger.FindAccount(F.Alice) != nullptr);
	CHECK(F.Ledger.FindAccount(FAccountId{999}) == nullptr);
	CHECK(F.Balance(F.Alice).IsZero());
}

EMPIRE_TEST(Ledger_JobRewardMovesMoneyFromWorldToPlayer)
{
	FFixture F;
	const FTransactionResult Result = F.Ledger.Transfer("job:1:payout", F.World, F.Alice, M(25000), ETransactionReason::JobReward, 3600);
	REQUIRE(Result.Succeeded());
	CHECK(!Result.bDuplicate);
	CHECK(Result.Id.IsValid());
	CHECK(F.Balance(F.Alice) == M(25000));
	CHECK(F.Balance(F.World) == M(-25000));
	CHECK(F.Ledger.GetCirculatingMoney() == M(25000));
	CHECK(F.Ledger.GetJournal().size() == 1);
	CHECK(F.Ledger.GetJournal()[0].GameTime == 3600);
}

EMPIRE_TEST(Ledger_DuplicateKeyNeverPaysTwice)
{
	FFixture F;
	const FTransactionResult First = F.Ledger.Transfer("job:1:payout", F.World, F.Alice, M(500), ETransactionReason::JobReward, 0);
	const FTransactionResult Second = F.Ledger.Transfer("job:1:payout", F.World, F.Alice, M(500), ETransactionReason::JobReward, 10);
	REQUIRE(First.Succeeded());
	REQUIRE(Second.Succeeded());
	CHECK(Second.bDuplicate);
	CHECK(Second.Id == First.Id);
	CHECK(F.Balance(F.Alice) == M(500));
	CHECK(F.Ledger.GetJournal().size() == 1);
}

EMPIRE_TEST(Ledger_ReusedKeyWithDifferentContentIsConflict)
{
	FFixture F;
	REQUIRE(F.Ledger.Transfer("job:1:payout", F.World, F.Alice, M(500), ETransactionReason::JobReward, 0).Succeeded());
	const FTransactionResult Result = F.Ledger.Transfer("job:1:payout", F.World, F.Alice, M(5000), ETransactionReason::JobReward, 0);
	CHECK(Result.Error == ELedgerError::IdempotencyConflict);
	CHECK(F.Balance(F.Alice) == M(500));
}

EMPIRE_TEST(Ledger_InsufficientFundsChangesNothing)
{
	FFixture F;
	F.Fund(F.Alice, 100, "fund");
	const FTransactionResult Result = F.Ledger.Transfer("buy:1", F.Alice, F.Bob, M(101), ETransactionReason::Purchase, 0);
	CHECK(Result.Error == ELedgerError::InsufficientFunds);
	CHECK(Result.OffendingAccount == F.Alice);
	CHECK(F.Balance(F.Alice) == M(100));
	CHECK(F.Balance(F.Bob) == M(0));
	CHECK(F.Ledger.GetJournal().size() == 1);
}

EMPIRE_TEST(Ledger_RejectedTransactionDoesNotConsumeKey)
{
	FFixture F;
	CHECK(F.Ledger.Transfer("buy:1", F.Alice, F.Bob, M(50), ETransactionReason::Purchase, 0).Error == ELedgerError::InsufficientFunds);
	F.Fund(F.Alice, 50, "fund");
	const FTransactionResult Retry = F.Ledger.Transfer("buy:1", F.Alice, F.Bob, M(50), ETransactionReason::Purchase, 0);
	CHECK(Retry.Succeeded());
	CHECK(!Retry.bDuplicate);
	CHECK(F.Balance(F.Bob) == M(50));
}

EMPIRE_TEST(Ledger_MultiPostingIsAtomicWhenOneLegFails)
{
	FFixture F;
	F.Fund(F.Alice, 1000, "fund");

	// Alice pays Bob and her company; the company leg also tries to debit Bob, who has nothing.
	FTransactionRequest Request;
	Request.IdempotencyKey = "split:1";
	Request.Postings = {{F.Alice, M(-1000)}, {F.AliceCo, M(1200)}, {F.Bob, M(-200)}};
	const FTransactionResult Result = F.Ledger.Commit(Request);

	CHECK(Result.Error == ELedgerError::InsufficientFunds);
	CHECK(Result.OffendingAccount == F.Bob);
	CHECK(F.Balance(F.Alice) == M(1000));
	CHECK(F.Balance(F.AliceCo) == M(0));
	CHECK(F.Balance(F.Bob) == M(0));
	CHECK(F.Ledger.VerifyIntegrity().bOk);
}

EMPIRE_TEST(Ledger_RejectsMalformedRequests)
{
	FFixture F;
	F.Fund(F.Alice, 1000, "fund");

	FTransactionRequest NoKey;
	NoKey.Postings = {{F.Alice, M(-1)}, {F.Bob, M(1)}};
	CHECK(F.Ledger.Commit(NoKey).Error == ELedgerError::MissingIdempotencyKey);

	FTransactionRequest Single;
	Single.IdempotencyKey = "single";
	Single.Postings = {{F.Alice, M(-1)}};
	CHECK(F.Ledger.Commit(Single).Error == ELedgerError::TooFewPostings);

	FTransactionRequest Unbalanced;
	Unbalanced.IdempotencyKey = "unbalanced";
	Unbalanced.Postings = {{F.Alice, M(-100)}, {F.Bob, M(150)}};
	CHECK(F.Ledger.Commit(Unbalanced).Error == ELedgerError::Unbalanced);

	FTransactionRequest ZeroLeg;
	ZeroLeg.IdempotencyKey = "zero";
	ZeroLeg.Postings = {{F.Alice, M(-100)}, {F.Bob, M(100)}, {F.AliceCo, M(0)}};
	CHECK(F.Ledger.Commit(ZeroLeg).Error == ELedgerError::ZeroPosting);

	FTransactionRequest SameAccountTwice;
	SameAccountTwice.IdempotencyKey = "twice";
	SameAccountTwice.Postings = {{F.Alice, M(-100)}, {F.Alice, M(100)}};
	CHECK(F.Ledger.Commit(SameAccountTwice).Error == ELedgerError::DuplicateAccount);

	FTransactionRequest Unknown;
	Unknown.IdempotencyKey = "unknown";
	Unknown.Postings = {{F.Alice, M(-100)}, {FAccountId{999}, M(100)}};
	CHECK(F.Ledger.Commit(Unknown).Error == ELedgerError::UnknownAccount);

	CHECK(F.Ledger.Transfer("neg", F.Alice, F.Bob, M(-5), ETransactionReason::Purchase, 0).Error == ELedgerError::InvalidAmount);
	CHECK(F.Ledger.Transfer("nil", F.Alice, F.Bob, M(0), ETransactionReason::Purchase, 0).Error == ELedgerError::InvalidAmount);

	CHECK(F.Balance(F.Alice) == M(1000));
	CHECK(F.Balance(F.Bob) == M(0));
	CHECK(F.Ledger.GetJournal().size() == 1);
}

EMPIRE_TEST(Ledger_OverflowIsRejected)
{
	FFixture F;
	constexpr FMoney::Rep Max = std::numeric_limits<FMoney::Rep>::max();
	REQUIRE(F.Ledger.Transfer("big", F.World, F.Alice, M(Max), ETransactionReason::Adjustment, 0).Succeeded());
	const FTransactionResult Result = F.Ledger.Transfer("more", F.World, F.Alice, M(1), ETransactionReason::Adjustment, 0);
	CHECK(Result.Error == ELedgerError::Overflow);
	CHECK(F.Balance(F.Alice) == M(Max));
}

EMPIRE_TEST(Ledger_CreditLineAllowsNegativeUpToLimit)
{
	FFixture F;
	const FAccountId Credit = F.Ledger.OpenAccount({"Alice Co Credit", EAccountKind::Business, {EOwnerType::Business, 10}, M(-500)});
	CHECK(F.Ledger.Transfer("draw:1", Credit, F.Bob, M(500), ETransactionReason::Purchase, 0).Succeeded());
	CHECK(F.Balance(Credit) == M(-500));
	CHECK(F.Ledger.Transfer("draw:2", Credit, F.Bob, M(1), ETransactionReason::Purchase, 0).Error == ELedgerError::InsufficientFunds);
}

EMPIRE_TEST(Ledger_InternalTransfersDoNotCreateMoney)
{
	FFixture F;
	F.Fund(F.Alice, 10000, "fund");
	const auto Before = F.Ledger.GetCirculatingMoney();

	// Round-tripping money between a player and their own company (salary, investment,
	// dividend) must never change the total amount of money in the economy.
	CHECK(F.Ledger.Transfer("invest:1", F.Alice, F.AliceCo, M(8000), ETransactionReason::Investment, 0).Succeeded());
	CHECK(F.Ledger.Transfer("salary:1", F.AliceCo, F.Alice, M(3000), ETransactionReason::Salary, 0).Succeeded());
	CHECK(F.Ledger.Transfer("dividend:1", F.AliceCo, F.Alice, M(5000), ETransactionReason::Dividend, 0).Succeeded());

	CHECK(F.Ledger.GetCirculatingMoney() == Before);
	CHECK(F.Balance(F.Alice) == M(10000));
	CHECK(F.Balance(F.AliceCo) == M(0));
}

EMPIRE_TEST(Ledger_VerifyIntegrityPassesOnConsistentState)
{
	FFixture F;
	F.Fund(F.Alice, 700, "fund");
	F.Ledger.Transfer("t1", F.Alice, F.Bob, M(200), ETransactionReason::Sale, 0);
	const FLedgerIntegrityReport Report = F.Ledger.VerifyIntegrity();
	CHECK(Report.bOk);
	CHECK(Report.Problems.empty());
}

// Randomized property test: thousands of random (often invalid) operations must never
// break the core invariants. Deterministic seed so failures are reproducible.
EMPIRE_TEST(Ledger_RandomOperationsPreserveInvariants)
{
	FLedger Ledger;
	std::vector<FAccountId> Ids;
	Ids.push_back(Ledger.OpenAccount({"World", EAccountKind::System, {EOwnerType::World, 0}}));
	for (std::uint64_t Index = 1; Index <= 8; ++Index)
	{
		const EAccountKind Kind = Index % 2 ? EAccountKind::Private : EAccountKind::Business;
		Ids.push_back(Ledger.OpenAccount({"P" + std::to_string(Index), Kind, {EOwnerType::Player, Index}}));
	}

	std::mt19937_64 Rng(20261027);
	std::uniform_int_distribution<std::size_t> PickAccount(0, Ids.size() - 1);
	std::uniform_int_distribution<FMoney::Rep> PickAmount(-500, 5000);
	std::uniform_int_distribution<int> PickKey(0, 2999);

	std::size_t Commits = 0;
	for (int Step = 0; Step < 20000; ++Step)
	{
		const FAccountId From = Ids[PickAccount(Rng)];
		const FAccountId To = Ids[PickAccount(Rng)];
		const FMoney Amount = M(PickAmount(Rng));
		const std::string Key = "k" + std::to_string(PickKey(Rng));

		const std::size_t JournalBefore = Ledger.GetJournal().size();
		const FTransactionResult Result = Ledger.Transfer(Key, From, To, Amount, ETransactionReason::Unspecified, Step);
		const std::size_t JournalAfter = Ledger.GetJournal().size();

		// The journal grows by exactly one on a fresh commit and never otherwise.
		const bool bFresh = Result.Succeeded() && !Result.bDuplicate;
		if (!CHECK(JournalAfter == JournalBefore + (bFresh ? 1u : 0u)))
		{
			return;
		}
		Commits += bFresh ? 1 : 0;
	}

	for (std::size_t Index = 1; Index < Ids.size(); ++Index)
	{
		CHECK(!Ledger.FindAccount(Ids[Index])->Balance.IsNegative());
	}

	const auto Circulating = Ledger.GetCirculatingMoney();
	const auto WorldBalance = FMoney::CheckedNegate(Ledger.FindAccount(Ids[0])->Balance);
	REQUIRE(Circulating.has_value());
	CHECK(Circulating == WorldBalance);
	CHECK(Commits > 100);
	CHECK(Ledger.VerifyIntegrity().bOk);
}
