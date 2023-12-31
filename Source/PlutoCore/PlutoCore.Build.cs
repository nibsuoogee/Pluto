using UnrealBuildTool;

public class PlutoCore : ModuleRules
{
	public PlutoCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnforceIWYU = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine",
		 "InputCore", "EnhancedInput", "AIModule" });
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
