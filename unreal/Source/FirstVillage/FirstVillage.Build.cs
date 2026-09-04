using UnrealBuildTool;

public class FirstVillage : ModuleRules
{
    public FirstVillage(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Json",
            "JsonUtilities",
            "AIModule",
            "NavigationSystem",
            "HTTP"
        });
    }
}
