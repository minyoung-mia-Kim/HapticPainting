// Fill out your copyright notice in the Description page of Project Settings.

#include "MainController.h"


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

// Called when the game starts or when spawned
void AMainController::BeginPlay()
{
	Super::BeginPlay();
	

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
	HManager->SetActorLocation(HManager->getHapticDevicePositionInUnrealCoordinates() + CurrentLocation);
	HManager->SetActorRotation(HManager->getHapticDeviceRotationAsUnrealRotator());

}

