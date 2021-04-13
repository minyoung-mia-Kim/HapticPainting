// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"
#include "TimerManager.h"
#include "Engine/Engine.h"



// Sets default values
AMainController::AMainController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainController::BeginPlay()
{
	Super::BeginPlay();
	PainterInstance = Cast<APainterPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	DHandler = GetWorld()->SpawnActor<ADrawingHandler>(ADrawingHandler::StaticClass());

	PainterInstance->FTexDelegate.AddDynamic(DHandler, &ADrawingHandler::ChangeBrushMode);

	PainterInstance->FMCInfoDelegate.AddDynamic(DHandler, &ADrawingHandler::Paint); // MC controller Drawing


}

// Called every frame
void AMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector location;

	//Bilboard with cam
	auto aa = GetWorld()->GetFirstPlayerController();
	location = aa->GetPawn()->GetActorLocation();

}



