using UnrealBuildTool;
using System.Collections.Generic;

public class FirstVillageTarget : TargetRules
{
    public FirstVillageTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("FirstVillage");
    }
}
