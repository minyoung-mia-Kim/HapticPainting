// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PainterPawn.h"
#include "GameFramework/GameModeBase.h"
#include "HapticDrawingGameModeBase.generated.h"

UCLASS()
class HAPTICDRAWING_API AHapticDrawingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AHapticDrawingGameModeBase();

	UPROPERTY()
	class APainterPawn* CurrentPawn;

	virtual void StartPlay() override;
	
	UPROPERTY()
		FString AssetName="";
};
