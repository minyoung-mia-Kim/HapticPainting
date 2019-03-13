// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Haptico/Public/HapticsManager.h"
#include "MainController.generated.h"

UCLASS()
class HAPTICDRAWING_API AMainController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainController();

protected:
	UPROPERTY(EditAnywhere)
	class AHapticsManager* HManager;
	UPROPERTY(EditAnywhere)
	class ADrawingHandler* DHandler;

	FVector CurrentLocation;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CalledBTN();
	
	
};
