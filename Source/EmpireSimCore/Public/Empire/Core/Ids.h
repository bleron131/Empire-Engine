// Empire Engine - engine-independent simulation core.
#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace empire
{

// Strongly typed numeric identifier. The tag type prevents mixing up ids of
// different entity kinds (e.g. passing an FAccountId where a FTransactionId is expected).
// Value 0 is reserved as "invalid".
template <typename Tag>
struct TStrongId
{
	std::uint64_t Value = 0;

	constexpr bool IsValid() const { return Value != 0; }
	constexpr auto operator<=>(const TStrongId&) const = default;
};

using FAccountId = TStrongId<struct FAccountIdTag>;
using FTransactionId = TStrongId<struct FTransactionIdTag>;

} // namespace empire

template <typename Tag>
struct std::hash<empire::TStrongId<Tag>>
{
	std::size_t operator()(const empire::TStrongId<Tag>& Id) const noexcept
	{
		return std::hash<std::uint64_t>{}(Id.Value);
	}
};
