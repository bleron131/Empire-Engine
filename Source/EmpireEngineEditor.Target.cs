// Empire Engine - editor target.

using UnrealBuildTool;

public class EmpireEngineEditorTarget : TargetRules
{
	public EmpireEngineEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		// See EmpireEngine.Target.cs for why "Latest" is used.
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("EmpireEngine");
	}
}
