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

	//Give the drawing plane position
	DHandler->receivedFbutton(HHandler->plane->GetComponentLocation(), HHandler->plane->GetComponentRotation(), hasClicked);
}

void AMainController::BindToSbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b2 clicked"));
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);

	//Give the drawing plane position
	DHandler->receivedSbutton(HHandler->plane->GetComponentLocation(), HHandler->plane->GetComponentRotation(), hasClicked);
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
	FVector location;
	FRotator rotation;

	auto aa = GetWorld()->GetFirstPlayerController();
	aa->GetPlayerViewPoint(location, rotation);
	rotation.Pitch = -rotation.Pitch; //updown
	rotation.Yaw += 180.f;			  //RL

	HHandler->SetActorLocation(rotation.RotateVector(HHandler->getHapticDevicePositionInUnrealCoordinates()) + location);
	HHandler->SetActorRotation(HHandler->getHapticDeviceRotationAsUnrealRotator()+rotation);

}

