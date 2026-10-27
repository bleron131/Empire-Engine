// Empire Engine - engine-independent simulation core as an Unreal module.
// NOT YET COMPILED: no Unreal Engine is available in the environment this was written in.
// The same sources are built and tested standalone via Tests/SimCore/CMakeLists.txt.

using UnrealBuildTool;

public class EmpireSimCore : ModuleRules
{
	public EmpireSimCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Only Core, for IMPLEMENT_MODULE. Simulation code itself must not depend on
		// UObject, Engine or any other Unreal module so it stays testable outside the editor.
		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		// The simulation core is written without exceptions and RTTI (Unreal defaults).
		bEnableExceptions = false;
		bUseRTTI = false;
	}
}
