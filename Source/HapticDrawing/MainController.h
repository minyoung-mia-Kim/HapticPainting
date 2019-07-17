// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "HapticHandler.h"
#include "DrawingHandler.h"
#include "ForceHandler.h"
#include "GameFramework/DefaultPawn.h"
#include "HapticDrawingGameModeBase.h"
#include "MainController.generated.h"


UCLASS(BlueprintType)
class HAPTICDRAWING_API AMainController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMainController();
	//Response Delegate
	UFUNCTION()
		void BindToFbuttonInput(FVector posDevice, bool hasClicked);

	UFUNCTION()
		void BindToSbuttonInput(FVector posDevice, bool hasClicked);

	UFUNCTION()
		void BindToBrushUpdate(float brushSize, FLinearColor brushColor, float viscosity, FString tex);

	/* Color Picker Input */
	UFUNCTION()
		void BindToBrushInput(FLinearColor selectedColor);

protected:
	UPROPERTY()
		class AHapticsHandler* HHandler;
	UPROPERTY()
		class ADrawingHandler* DHandler;
	UPROPERTY()
		class AForceHandler* FHandler;

	FVector CurrentLocation;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Adjust Haptic Handler position */
	FRotator DefaultPosition = FRotator(0.0f, 180.f, 0.0f);
	FRotator DefaultDirection = FRotator(0.0f, 180.0f, 0.0f);


	UFUNCTION()
		void SetHapticTurn(FRotator rotator);

	UFUNCTION()
		void SetHapticForce(FVector hForce);


};

