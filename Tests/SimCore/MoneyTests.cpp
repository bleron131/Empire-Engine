// Empire Engine - tests for FMoney.
#include "Framework/TestFramework.h"

#include "Empire/Core/Money.h"

#include <limits>

using namespace empire;

namespace
{
constexpr FMoney::Rep MaxRep = std::numeric_limits<FMoney::Rep>::max();
constexpr FMoney::Rep MinRep = std::numeric_limits<FMoney::Rep>::min();
}

EMPIRE_TEST(Money_DefaultIsZero)
{
	CHECK(FMoney().IsZero());
	CHECK(FMoney() == FMoney::Zero());
	CHECK(!FMoney().IsNegative());
	CHECK(!FMoney().IsPositive());
}

EMPIRE_TEST(Money_CheckedAddRegular)
{
	const auto Sum = FMoney::CheckedAdd(FMoney::FromMinor(150), FMoney::FromMinor(-50));
	REQUIRE(Sum.has_value());
	CHECK(Sum->Minor() == 100);
}

EMPIRE_TEST(Money_CheckedAddDetectsOverflow)
{
	CHECK(!FMoney::CheckedAdd(FMoney::FromMinor(MaxRep), FMoney::FromMinor(1)).has_value());
	CHECK(!FMoney::CheckedAdd(FMoney::FromMinor(MinRep), FMoney::FromMinor(-1)).has_value());
	CHECK(FMoney::CheckedAdd(FMoney::FromMinor(MaxRep), FMoney::FromMinor(MinRep)).has_value());
}

EMPIRE_TEST(Money_CheckedSubDetectsOverflowAtBoundaries)
{
	CHECK(!FMoney::CheckedSub(FMoney::FromMinor(MinRep), FMoney::FromMinor(1)).has_value());
	CHECK(!FMoney::CheckedSub(FMoney::FromMinor(0), FMoney::FromMinor(MinRep)).has_value());

	// -1 - MIN == MAX is representable and must not be rejected.
	const auto Edge = FMoney::CheckedSub(FMoney::FromMinor(-1), FMoney::FromMinor(MinRep));
	REQUIRE(Edge.has_value());
	CHECK(Edge->Minor() == MaxRep);
}

EMPIRE_TEST(Money_CheckedNegate)
{
	CHECK(!FMoney::CheckedNegate(FMoney::FromMinor(MinRep)).has_value());
	const auto Negated = FMoney::CheckedNegate(FMoney::FromMinor(MaxRep));
	REQUIRE(Negated.has_value());
	CHECK(Negated->Minor() == -MaxRep);
}

EMPIRE_TEST(Money_Ordering)
{
	CHECK(FMoney::FromMinor(-1) < FMoney::Zero());
	CHECK(FMoney::FromMinor(10) > FMoney::FromMinor(9));
}
