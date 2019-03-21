// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"
#include "TimerManager.h"


// Sets default values
AMainController::AMainController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMainController::BindToFbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b1 clicked %d"), hasClicked);
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	DHandler->receivedFbutton(posDevice, HHandler->GetActorRotation(), hasClicked);
}

void AMainController::BindToSbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b2 clicked"));
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	DHandler->receivedSbutton(posDevice, HHandler->GetActorRotation(), hasClicked);
}

// Called when the game starts or when spawned
void AMainController::BeginPlay()
{
	Super::BeginPlay();
	HHandler = GetWorld()->SpawnActor<AHapticsHandler>(AHapticsHandler::StaticClass());

	HHandler->FbuttonInputDelegate.AddDynamic(this, &AMainController::BindToFbuttonInput);
	HHandler->SbuttonInputDelegate.AddDynamic(this, &AMainController::BindToSbuttonInput);

	DHandler = GetWorld()->SpawnActor<ADrawingHandler>(ADrawingHandler::StaticClass());

}

// Called every frame
void AMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentLocation = GetActorLocation();
	HHandler->SetActorLocation(HHandler->getHapticDevicePositionInUnrealCoordinates());
	HHandler->SetActorRotation(HHandler->getHapticDeviceRotationAsUnrealRotator());

}

