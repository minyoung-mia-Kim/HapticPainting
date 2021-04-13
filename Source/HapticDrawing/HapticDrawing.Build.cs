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

        /* VR Required Modules */
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SteamVR", "HeadMountedDisplay" });
        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "SteamVR" });

        /* Prpcedural Mesh and Static Mesh */
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RawMesh" });
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent" });
        
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject" });

        // Convert procedural mesh component to static mesh component
        ////
        PublicDependencyModuleNames.AddRange(new string[] { "AssetTools"});
        ////

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
