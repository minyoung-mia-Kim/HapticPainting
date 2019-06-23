// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ForceHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHapticForceDelegate, FVector, hForce);


class UStaticMeshComponent;

UCLASS()
class HAPTICDRAWING_API AForceHandler : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, Category = "ForceProxy")
		UStaticMeshComponent* Proxy;
	UPROPERTY()
		FVector MeshPosition;
	UPROPERTY()
		FVector MeshNormal;
	UPROPERTY()
		FVector MeshTangent;

	UPROPERTY()
		FVector CursorPosition;
	UPROPERTY()
		FVector CurrentForce;
	UPROPERTY()
		FRotator HHandlerRotator;

	float SPRING_TOLERANCE = 0.000000005;
	float springK = 1.2f;
	float ForceClamp = 1.f;


	UPROPERTY()
		FHapticForceDelegate HapticForceUpdate;

	// Sets default values for this actor's properties
	AForceHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	float ClampForce(float force);
	UFUNCTION()
		void getForceInfo(FVector msPosition, FVector msNormal, FVector msTangent, FVector cPosition);
	UFUNCTION()
		void cleanForceInfo();
	
	
};
