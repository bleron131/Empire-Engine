// Empire Engine - verifies that the simulation core is linked and usable from Unreal.
// The full rule tests live in Tests/SimCore; this test only proves the integration works.
#include "Misc/AutomationTest.h"

#include "Empire/Economy/Ledger.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmpireSimCoreLedgerSmokeTest, "Empire.SimCore.LedgerSmoke",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmpireSimCoreLedgerSmokeTest::RunTest(const FString& Parameters)
{
	empire::FLedger Ledger;
	const empire::FAccountId World =
		Ledger.OpenAccount({"World", empire::EAccountKind::System, {empire::EOwnerType::World, 0}});
	const empire::FAccountId Player =
		Ledger.OpenAccount({"Player", empire::EAccountKind::Private, {empire::EOwnerType::Player, 1}});

	const empire::FMoney Reward = empire::FMoney::FromMinor(2500);
	const empire::FTransactionResult First =
		Ledger.Transfer("smoke:payout", World, Player, Reward, empire::ETransactionReason::JobReward, 0);
	const empire::FTransactionResult Retry =
		Ledger.Transfer("smoke:payout", World, Player, Reward, empire::ETransactionReason::JobReward, 0);

	TestTrue(TEXT("Payout is committed"), First.Succeeded());
	TestTrue(TEXT("Retry is detected as duplicate"), Retry.bDuplicate);
	TestEqual(TEXT("Player balance"), static_cast<int64>(Ledger.FindAccount(Player)->Balance.Minor()), static_cast<int64>(2500));
	TestTrue(TEXT("Ledger integrity"), Ledger.VerifyIntegrity().bOk);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
