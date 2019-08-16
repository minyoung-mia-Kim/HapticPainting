// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorPicker.h"
#include "Engine/World.h"
#include "HeadMountedDisplay.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PainterPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnInfoDelegate, FRotator, pawnRotator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSBrushUpdateDelegate, FLinearColor, sColor, float, sSize);
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
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UMotionControllerComponent* MC_Right;*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UCameraComponent* VRcamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		USpringArmComponent* SPArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MyPawn")
		USceneComponent* Center;


	/* VR Controller */
	UFUNCTION(BlueprintCallable)
		float TurnLeft();

	UFUNCTION(BlueprintCallable)
		float TurnRight();

	UPROPERTY(BlueprintCallable)
		FPawnInfoDelegate FPawnUpdateDelegate;

	UPROPERTY(BlueprintCallable)
		FVRInputDelegate FVRInputTriggerDelegate;

	UPROPERTY(BlueprintCallable)
		FSBrushUpdateDelegate FSelectedBrushUpdateDelegate;

	UPROPERTY(BlueprintCallable)
		FActivateHapticDelegate FActivateVDPDelegate;

	/* Color picker */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ColorPicker")
		AColorPicker* Cpicker;
	UFUNCTION()
		void Color(FLinearColor sColor, float sSize);

	UFUNCTION()
		void ActivateVDP();


	//UFUNCTION()
		//FString getHMDType();

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
	//UFUNCTION(BlueprintCallable, Category = "Pawn")
	//	void TurnAtRate(float Rate);

	//UFUNCTION(BlueprintCallable, Category = "Pawn")
	//	void LookUpAtRate(float Rate);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
