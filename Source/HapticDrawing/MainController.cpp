// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"
#include "TimerManager.h"


// Sets default values
AMainController::AMainController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//
	//FVector NewLocation = GetActorLocation() + FVector(5.f, 0.f, 0.f);

	//HM_Cursor = CreateDefaultSubobject<USphereComponent>(TEXT("HM_Cursor"));
	//HM_Cursor->InitSphereRadius(100.f);	

	//HM_Transform = CreateDefaultSubobject<USceneComponent>(TEXT("HM_Transform"));
}

void AMainController::BindToFbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b1 clicked %d"), hasClicked);
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	DHandler->receivedFbutton(HHandler->GetActorLocation(), HHandler->GetActorRotation(), hasClicked);
}

void AMainController::BindToSbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b2 clicked"));
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	DHandler->receivedSbutton(HHandler->GetActorLocation(), HHandler->GetActorRotation(), hasClicked);
}

// Called when the game starts or when spawned
void AMainController::BeginPlay()
{
	Super::BeginPlay();
	HHandler->FbuttonInputDelegate.AddDynamic(this, &AMainController::BindToFbuttonInput);
	HHandler->SbuttonInputDelegate.AddDynamic(this, &AMainController::BindToSbuttonInput);
	DHandler = GetWorld()->SpawnActor<ADrawingHandler>(ADrawingHandler::StaticClass());


	/*FVector NewLocation = GetActorLocation() + FVector(5.f, 0.f, 0.f);
	HManager = GetWorld()->SpawnActor<AHapticsManager>(AHapticsManager::StaticClass(), NewLocation, FRotator::ZeroRotator);
	HManager->SetRootComponent(HM_Transform);

	if (HManager)
		UE_LOG(LogTemp, Warning, TEXT("HManager Generated"));

	if (HManager->HasValidRootComponent())
		UE_LOG(LogTemp, Warning, TEXT("has rc"));
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	auto RC = HManager->GetRootComponent();
	HM_Cursor->AttachToComponent(RC, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HM_Transform"));*/

}

// Called every frame
void AMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentLocation = GetActorLocation();
	HHandler->SetActorLocation(HHandler->getHapticDevicePositionInUnrealCoordinates() + CurrentLocation);
	HHandler->SetActorRotation(HHandler->getHapticDeviceRotationAsUnrealRotator());

}

