// Empire Engine - engine-independent simulation core.
#pragma once

#include <compare>
#include <cstdint>
#include <limits>
#include <optional>

namespace empire
{

// Amount of money in minor currency units (e.g. cents).
// Integer-only so economic calculations are deterministic across machines
// (host and clients) and never accumulate floating-point drift.
// Can be negative: it is used for balances as well as signed ledger deltas.
// numeric_limits calls are parenthesized so a Windows min/max macro cannot break them.
class FMoney
{
public:
	using Rep = std::int64_t;

	constexpr FMoney() = default;

	static constexpr FMoney FromMinor(Rep Minor) { return FMoney(Minor); }
	static constexpr FMoney Zero() { return FMoney(0); }

	constexpr Rep Minor() const { return Value; }
	constexpr bool IsZero() const { return Value == 0; }
	constexpr bool IsNegative() const { return Value < 0; }
	constexpr bool IsPositive() const { return Value > 0; }

	constexpr auto operator<=>(const FMoney&) const = default;

	// Overflow-checked arithmetic. Returns std::nullopt instead of wrapping.
	static constexpr std::optional<FMoney> CheckedAdd(FMoney A, FMoney B)
	{
		constexpr Rep Max = (std::numeric_limits<Rep>::max)();
		constexpr Rep Min = (std::numeric_limits<Rep>::min)();
		if ((B.Value > 0 && A.Value > Max - B.Value) || (B.Value < 0 && A.Value < Min - B.Value))
		{
			return std::nullopt;
		}
		return FMoney(A.Value + B.Value);
	}

	static constexpr std::optional<FMoney> CheckedSub(FMoney A, FMoney B)
	{
		constexpr Rep Max = (std::numeric_limits<Rep>::max)();
		constexpr Rep Min = (std::numeric_limits<Rep>::min)();
		if ((B.Value > 0 && A.Value < Min + B.Value) || (B.Value < 0 && A.Value > Max + B.Value))
		{
			return std::nullopt;
		}
		return FMoney(A.Value - B.Value);
	}

	static constexpr std::optional<FMoney> CheckedNegate(FMoney A)
	{
		if (A.Value == (std::numeric_limits<Rep>::min)())
		{
			return std::nullopt;
		}
		return FMoney(-A.Value);
	}

private:
	explicit constexpr FMoney(Rep InValue) : Value(InValue) {}

	Rep Value = 0;
};

} // namespace empire
