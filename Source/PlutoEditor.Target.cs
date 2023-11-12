using UnrealBuildTool;

public class PlutoEditorTarget : TargetRules
{
	public PlutoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "PlutoCore" } );
	}
}
