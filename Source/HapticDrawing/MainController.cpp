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

void AMainController::BindToFbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b1 clicked %d"), hasClicked);
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);

	//Give the drawing brush position
	DHandler->receivedFbutton(HHandler->brush->GetComponentLocation(), HHandler->brush->GetComponentRotation(), hasClicked);
}

void AMainController::BindToSbuttonInput(FVector posDevice, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("I'm Controller b2 clicked"));
	//UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);

	//Give the drawing brush position
	DHandler->receivedSbutton(HHandler->brush->GetComponentLocation(), HHandler->brush->GetComponentRotation(), hasClicked);
}

void AMainController::BindToBrushUpdate(float brushSize, FLinearColor brushColor)
{
	HHandler->RefreshBrushCursor(brushSize, brushColor);
	//UE_LOG(LogTemp, Warning, TEXT("middle Color: %s"), *(brushColor.ToString()));

}



// Called when the game starts or when spawned
void AMainController::BeginPlay()
{
	Super::BeginPlay();
	APainterPawn* PainterInstance = Cast<APainterPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	PainterInstance->FPawnUpdateDelegate.AddDynamic(this, &AMainController::SetHapticTurn);
	HHandler = GetWorld()->SpawnActor<AHapticsHandler>(AHapticsHandler::StaticClass());

	HHandler->FbuttonInputDelegate.AddDynamic(this, &AMainController::BindToFbuttonInput);
	HHandler->SbuttonInputDelegate.AddDynamic(this, &AMainController::BindToSbuttonInput);
	HHandler->AttachToActor(PainterInstance, FAttachmentTransformRules::KeepRelativeTransform);

	DHandler = GetWorld()->SpawnActor<ADrawingHandler>(ADrawingHandler::StaticClass());
	DHandler->FBrushUpdateDelegate.AddDynamic(this, &AMainController::BindToBrushUpdate);

}

// Called every frame
void AMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector location;
	FRotator rotation;

	//Bilboard with cam
	auto aa = GetWorld()->GetFirstPlayerController();
	location = aa->GetPawn()->GetActorLocation();
	rotation = aa->GetPawn()->GetActorRotation();

	//aa->GetPlayerViewPoint(location, rotation);
	////rotation.Pitch = -rotation.Pitch; //updown
	////rotation.Yaw += 180.f;			  //RL

	//Follow the pawn's camera view
	//HHandler->SetActorLocation(rotation.RotateVector(HHandler->getHapticDevicePositionInUnrealCoordinates()));

	/* Follow the pawn's location*/
	//DefaultPosition = DefaultPosition + rotation;
	HHandler->SetActorLocation(DefaultPosition.RotateVector(HHandler->getHapticDevicePositionInUnrealCoordinates()) + location);
	HHandler->SetActorRotation(DefaultDirection + HHandler->getHapticDeviceRotationAsUnrealRotator());
	//HHandler->SetCursorRotation(rotation);
}

void AMainController::SetHapticTurn(FRotator rotator)
{
	DefaultPosition += rotator;
	DefaultDirection += rotator;
	UE_LOG(LogTemp, Warning, TEXT("DP Yaw :%f"), DefaultPosition.Yaw);
	UE_LOG(LogTemp, Warning, TEXT("DD Yaw :%f"), DefaultDirection.Yaw);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Yaw :%f"), DefaultPosition.Yaw));
	//HHandler->SetCursorRotation(rotator);
}

