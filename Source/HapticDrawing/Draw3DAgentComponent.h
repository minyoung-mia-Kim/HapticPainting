// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ActorComponent.h"
#include "SceneView.h"
#include "Draw3DAgentComponent.generated.h"


UENUM(BlueprintType)
enum class ESceneDepthPriorityGroupProxy : uint8
{
	/** World scene DPG. */
	SDPGP_World UMETA(DisplayName = "World"),
	/** Foreground scene DPG. */
	SDPGP_Foreground UMETA(DisplayName = "Foreground")
};

USTRUCT(BlueprintType)
struct HAPTICDRAWING_API FPrimitiveDrawProxy
{
	GENERATED_BODY()

public:
	FPrimitiveDrawProxy() : PDI(nullptr) {}
	FPrimitiveDrawProxy(class FPrimitiveDrawInterface* PDI) : PDI(PDI) {}

	class FPrimitiveDrawInterface* PDI;
};


DECLARE_DELEGATE_ThreeParams(FCreateDynMeshElementDelegate, const FSceneView*, class FPrimitiveDrawInterface*, const FMatrix&);

/**
* Draw 3D primitive agent component.
* It helps you draw 3D primitives easy, and it is very light-weight drawing component.
*/
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class HAPTICDRAWING_API UDraw3DAgentComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Draw3DAgent")
		bool bDrawOnlyIfSelected;

	FCreateDynMeshElementDelegate CreateDynMeshElemDelegate;

public:
	UDraw3DAgentComponent(const FObjectInitializer& ObjectInitializer);

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.

	UFUNCTION(BlueprintCallable, Category = "Draw3DAgent")
		void Refresh();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Draw3DAgent")
		void CreateDynamicElem(FPrimitiveDrawProxy PDP);

public:
	static void GetCameraAxis(const FSceneView* View, FVector& CameraX, FVector& CameraY);

	UFUNCTION(BlueprintCallable, Category = "PrimitiveDrawProxy")
		static void DrawLine(
			FPrimitiveDrawProxy PDP,
			const FVector& Start,
			const FVector& End,
			const FLinearColor& Color,
			ESceneDepthPriorityGroupProxy DepthPriority,
			float Thickness = 0.0f,
			float DepthBias = 0.0f,
			bool bScreenSpace = false
		);

	UFUNCTION(BlueprintCallable, Category = "PrimitiveDrawProxy")
		static void DrawPoint(
			FPrimitiveDrawProxy PDP,
			const FVector& Position,
			const FLinearColor& Color,
			float PointSize,
			ESceneDepthPriorityGroupProxy DepthPriority
		);

	UFUNCTION(BlueprintCallable, Category = "PrimitiveDrawProxy")
		void DrawCircle(
			FPrimitiveDrawProxy PDP,
			const FVector& Base,
			const FVector& X,
			const FVector& Y,
			const FLinearColor& Color,
			float Radius,
			int32 NumSides,
			ESceneDepthPriorityGroupProxy DepthPriority,
			float Thickness = 0.0f,
			float DepthBias = 0.0f,
			bool bScreenSpace = false);

	// Other Draw function wrappers...
};
