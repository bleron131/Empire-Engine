// Empire Engine - test runner for the standalone simulation core build.
#include "Framework/TestFramework.h"

#include <cstdio>
#include <cstring>

// Usage: EmpireSimCoreTests [name-filter]
int main(int Argc, char** Argv)
{
	const char* Filter = Argc > 1 ? Argv[1] : nullptr;
	int Run = 0;
	int Failed = 0;

	for (const empire::test::FTestCase& Test : empire::test::Registry())
	{
		if (Filter && !std::strstr(Test.Name, Filter))
		{
			continue;
		}
		empire::test::FContext Context;
		Test.Function(Context);
		++Run;
		const bool bPassed = Context.Failures == 0;
		Failed += bPassed ? 0 : 1;
		std::printf("[%s] %s (%d assertions)\n", bPassed ? "PASS" : "FAIL", Test.Name, Context.Assertions);
	}

	std::printf("\n%d tests run, %d failed\n", Run, Failed);
	return (Failed == 0 && Run > 0) ? 0 : 1;
}
