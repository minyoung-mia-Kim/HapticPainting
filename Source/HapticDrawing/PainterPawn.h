// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/World.h"
#include "HeadMountedDisplay.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "PainterPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnInfoDelegate, FRotator, pawnRotator);
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
	UPROPERTY(EditDefaultsOnly, Category = "MyPawn")
		UMotionControllerComponent* MC_Left;
	UPROPERTY(EditDefaultsOnly, Category = "MyPawn")
		UMotionControllerComponent* MC_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MyPawn")
		UCameraComponent* VRcamera;


	/* VR Controller */
	UFUNCTION(BlueprintCallable)
		float TurnLeft();

	UFUNCTION(BlueprintCallable)
		float TurnRight();

	UPROPERTY(BlueprintCallable)
		FPawnInfoDelegate FPawnUpdateDelegate;

	UFUNCTION(BlueprintCallable)
		void RotateBrush();

	//UFUNCTION()
		//FString getHMDType();

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveForward(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveRight(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveUp_World(float Val);


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
