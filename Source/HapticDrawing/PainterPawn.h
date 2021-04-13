// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorPicker.h"
#include "Engine/World.h"
#include "HeadMountedDisplay.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "PainterPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnInfoDelegate, FRotator, pawnRotator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSBrushTextureDelegate, int, nTex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSBrushUpdateDelegate, FLinearColor, sColor, float, sSize);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRightMCUpdataDelegate, FVector, position, FRotator, rotation, bool,TriggerReleased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVRInputDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActivateHapticDelegate);



class UMotionControllerComponent;

UCLASS()
class HAPTICDRAWING_API APainterPawn : public APawn
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	APainterPawn();
	UPROPERTY(EditDefaultsOnly, Category = "MyPawn")
		USceneComponent* RComponent;
	/* VR */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UMotionControllerComponent* MC_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UMotionControllerComponent* MC_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		USphereComponent* cursor = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UStaticMeshComponent* SM_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UCameraComponent* VRcamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		USpringArmComponent* SPArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		URotatingMovementComponent* RMComponent;


	/* VR Controller */
	UFUNCTION(BlueprintCallable)
		float TurnLeft();

	UFUNCTION(BlueprintCallable)
		float TurnRight();

	UPROPERTY(BlueprintCallable)
		FPawnInfoDelegate FPawnUpdateDelegate;

	UPROPERTY(BlueprintCallable)
		FSBrushTextureDelegate FTexDelegate;

	UPROPERTY(BlueprintCallable)
	FVRInputDelegate FVRInputTriggerDelegate;

	UPROPERTY(BlueprintCallable)
		FSBrushUpdateDelegate FSelectedBrushUpdateDelegate;

	UPROPERTY(BlueprintCallable)
		FActivateHapticDelegate FActivateVDPDelegate;

	/* alert the information of right - motion controller to others*/
	UPROPERTY(BlueprintCallable)
		FRightMCUpdataDelegate FMCInfoDelegate;
	UFUNCTION()
		void TriggerReleased();
	UFUNCTION()
		void TriggerPressed();
	bool bTriggerReleased;
	bool bTriggerReleasedFirst;

	/* Temporary Variables for Testing without HMD */
	int32 x_length ;
	int32 z_length ;

	/* Color picker */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ColorPicker")
		AColorPicker* Cpicker;
	UFUNCTION()
		void Color(FLinearColor sColor, float sSize);
	UFUNCTION()
		void Texture(int tex);
	UFUNCTION()
		void ActivateVDP();


	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveForward(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveRight(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveUp_World(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void GetHapticCursor(FVector Pos);

	FVector PrevCursorPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Rotation")
		float RotationAngle;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
