// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class HordeModeEditor : ModuleRules
{
	public HordeModeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { "HordeModeEditor/Public" });

        PrivateIncludePaths.AddRange(new string[]  { "HordeModeEditor/Private" });
	
		PublicDependencyModuleNames.AddRange(new string[] { "HordeMode", "UnrealEd", "Core", "CoreUObject", "Engine", "InputCore" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "DetailCustomizations", "PropertyEditor", "LevelEditor"  });
    }
}
