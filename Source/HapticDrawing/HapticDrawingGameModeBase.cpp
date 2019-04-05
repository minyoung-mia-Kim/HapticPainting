// Fill out your copyright notice in the Description page of Project Settings.

#include "HapticDrawingGameModeBase.h"


AHapticDrawingGameModeBase::AHapticDrawingGameModeBase()
{
	//DefaultPawnClass
	DefaultPawnClass = APainterPawn::StaticClass();
	CurrentPawn = Cast<APainterPawn>((*DefaultPawnClass)->GetDefaultObject<APawn>());
}
void AHapticDrawingGameModeBase::StartPlay()
{
	Super::StartPlay();

}

