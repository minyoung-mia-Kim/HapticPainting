// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Configs.generated.h"

/**
 * 
 */
UCLASS()
class HAPTICDRAWING_API AConfigs : public AWorldSettings
{
	GENERATED_BODY()
	
private:
	
public:
	AConfigs();
	bool bDeviceConnected;

};
