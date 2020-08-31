// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomInstance.generated.h"

/**
 * 
 */
UCLASS()
class HAPTICDRAWING_API UCustomInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
//	UCustomInstance(const FObjectInitilizer & ObjectInitilizer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString AssetName = "";
};
