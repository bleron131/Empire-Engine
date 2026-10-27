// Empire Engine - primary game module (Unreal-side gameplay code).

using UnrealBuildTool;

public class EmpireEngine : ModuleRules
{
	public EmpireEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
		});

		// Engine-independent simulation rules (economy, ledger, ...). Gameplay code calls into
		// it; it never calls back into Unreal.
		PrivateDependencyModuleNames.Add("EmpireSimCore");
	}
}
