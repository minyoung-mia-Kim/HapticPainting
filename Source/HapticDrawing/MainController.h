// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawingHandler.h"
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

protected:
	UPROPERTY()
		class ADrawingHandler* DHandler;
	UPROPERTY()
		class APainterPawn* PainterInstance;

	FVector CurrentLocation;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

