using UnrealBuildTool;

public class PlutoTarget : TargetRules
{
	public PlutoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "PlutoCore" } );
	}
}
