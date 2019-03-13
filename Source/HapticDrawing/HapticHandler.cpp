// Fill out your copyright notice in the Description page of Project Settings.



#include "HapticHandler.h"
#include "IHaptico.h"
#include "AsyncWork.h"
#include "HapticThread.h"
#include "HapticThreadInput.h"
#include "HapticThreadOutput.h"

/**
 * constructs an instance of the haptic manager
*/
AHapticsHandler::AHapticsHandler()
{
	PrimaryActorTick.bCanEverTick = true;
}

/**
 * Called when the actor is spawned and starts the haptic thread
 */
void AHapticsHandler::BeginPlay()
{
	Super::BeginPlay();
	UHapticThreadInput::getInst().setRunThread(true);
	(new FAutoDeleteAsyncTask<FHapticThread>(IHaptico::Get(), this))->StartBackgroundTask();
}

/**
* Called when the actor is destroyed and ends the haptic thread
*/
void  AHapticsHandler::EndPlay(EEndPlayReason::Type type)
{
	Super::EndPlay(type);
	UHapticThreadInput::getInst().setRunThread(false);
}

/**
 * Called every frame
*/
void AHapticsHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * set the force that should be applied to the haptic device in the next tick of the haptic thread
*/
void AHapticsHandler::setForceToApply(FVector force) {
	UHapticThreadInput::getInst().setForceToApply(force);
}

/**
* set the torque that should be applied to the haptic device in the next tick of the haptic thread
*/
void AHapticsHandler::setTorqueToApply(FVector torque) {
	UHapticThreadInput::getInst().setTorqueToApply(torque);
}

/**
* gets the current position of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDevicePosition() {
	return UHapticThreadOutput::getInst().getHapticCursorPosition();
}

/**
* get the current linear velocity of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDeviceLinearVelocity() {
	return UHapticThreadOutput::getInst().getHapticCursorLinearVelocity();
}

/**
* get the current angular velocity of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDeviceAngularVelocity() {
	return UHapticThreadOutput::getInst().getHapticCursorAngularVelocity();
}

/**
* get the current rotation of the haptic device end affector
*/
FMatrix AHapticsHandler::getHapticDeviceRotation() {
	return UHapticThreadOutput::getInst().getHapticCursorRotation();
}

/**
* get the current rotation of the haptic device end affector as an unreal rotator
*/
FRotator AHapticsHandler::getHapticDeviceRotationAsUnrealRotator() {
	FMatrix rotation = UHapticThreadOutput::getInst().getHapticCursorRotation();
	FVector euler = rotation.Rotator().Euler();
	return FRotator(-euler.Y, -euler.Z, euler.X);
}

/**
* get the current position of the haptic device end affector in unreal coordinates
*/
FVector AHapticsHandler::getHapticDevicePositionInUnrealCoordinates() {
	FVector position = UHapticThreadOutput::getInst().getHapticCursorPosition();
	return FVector(position.X * 1000, -position.Y * 1000, position.Z * 1000);
}

/**
* broad casts the new haptic data as a multicast delegate
*/
void AHapticsHandler::broadCastNewHapticData(FVector position, FMatrix rotation, FVector linearVelocity, FVector angularVelocity) {
	OnHapticHandlerTick.Broadcast(position, rotation, linearVelocity, angularVelocity);
}
void AHapticsHandler::button1Clicked()
{
	FVector position = this->getHapticDevicePositionInUnrealCoordinates();
	UE_LOG(LogTemp, Warning, TEXT("b1 clicked"));
	UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	isFirstClicked = true;
}

void AHapticsHandler::button2Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("b2 clicked"));
}