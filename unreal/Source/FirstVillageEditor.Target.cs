using UnrealBuildTool;
using System.Collections.Generic;

public class FirstVillageEditorTarget : TargetRules
{
    public FirstVillageEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("FirstVillage");
    }
}
