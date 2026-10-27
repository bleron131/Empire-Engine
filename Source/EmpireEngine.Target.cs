// Empire Engine - game target.

using UnrealBuildTool;

public class EmpireEngineTarget : TargetRules
{
	public EmpireEngineTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		// "Latest" instead of a pinned version so the project builds with the installed 5.x
		// engine without edits. Pin explicitly once the engine version is fixed for release.
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("EmpireEngine");
	}
}
