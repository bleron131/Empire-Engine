// Empire Engine - minimal test framework for the standalone simulation core build.
// Dependency-free and exception-free, matching the core's compile settings.
#pragma once

#include <cstdio>
#include <vector>

namespace empire::test
{

class FContext
{
public:
	bool Check(bool bCondition, const char* Expression, const char* File, int Line)
	{
		++Assertions;
		if (!bCondition)
		{
			++Failures;
			std::printf("    FAILED %s:%d: %s\n", File, Line, Expression);
		}
		return bCondition;
	}

	int Assertions = 0;
	int Failures = 0;
};

using FTestFunction = void (*)(FContext&);

struct FTestCase
{
	const char* Name;
	FTestFunction Function;
};

inline std::vector<FTestCase>& Registry()
{
	static std::vector<FTestCase> Tests;
	return Tests;
}

struct FRegistrar
{
	FRegistrar(const char* Name, FTestFunction Function) { Registry().push_back({Name, Function}); }
};

} // namespace empire::test

#define EMPIRE_TEST(Name)                                                         \
	static void Name(::empire::test::FContext& TestContext);                      \
	static const ::empire::test::FRegistrar Name##Registrar(#Name, &Name);        \
	static void Name(::empire::test::FContext& TestContext)

// Records a failure and continues.
#define CHECK(Expression) TestContext.Check(static_cast<bool>(Expression), #Expression, __FILE__, __LINE__)

// Records a failure and leaves the test (no exceptions available).
#define REQUIRE(Expression)                                                       \
	do                                                                            \
	{                                                                             \
		if (!CHECK(Expression))                                                   \
		{                                                                         \
			return;                                                               \
		}                                                                         \
	} while (false)
