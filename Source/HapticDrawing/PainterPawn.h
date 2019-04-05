// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/World.h"
#include "PainterPawn.generated.h"

UCLASS()
class HAPTICDRAWING_API APainterPawn : public ADefaultPawn
{
	GENERATED_BODY()


public:
	// Sets default values for this pawn's properties
	APainterPawn();
	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveForward(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveRight(float Val);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		virtual void MoveUp_World(float Val);


	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void TurnAtRate(float Rate);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
		void LookUpAtRate(float Rate);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
