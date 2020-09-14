// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class HapticDrawing : ModuleRules
{
	public HapticDrawing(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "SlateCore","Slate"});
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RenderCore"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Haptico" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Haptico" });
        PrivateIncludePathModuleNames.AddRange(new string[] { "Haptico" });
        PublicIncludePathModuleNames.AddRange(new string[] { "Haptico" });

        /* VR Required Modules */
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SteamVR", "SteamVRController", "HeadMountedDisplay" });
        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "SteamVR" });

        // Convert procedural mesh component to static mesh component
        ////
        PublicDependencyModuleNames.AddRange(new string[] { "RawMesh", "AssetTools"});
        ////

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
