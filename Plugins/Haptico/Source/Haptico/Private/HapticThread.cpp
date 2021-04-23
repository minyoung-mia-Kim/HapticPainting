/* Copyright (C) 2018 Sinan Demirtas
*
* This software may be distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/
#include "IHaptico.h"
#include "HapticThread.h"
#include "HapticDeviceButtonHandler.h"
#include "HapticsManager.h"
#include "Async.h"
#include "HapticThreadOutput.h"
#include "HapticThreadInput.h"

/**
* this is the haptic thread that retrieves the data from the device and sets forces and torques
*/
void FHapticThread::DoWork()
{
	HapticDeviceButtonHandler buttonHandler(&haptico);
	bool status = haptico.connect();
	UHapticThreadOutput::getInst().setDeviceConnectStatus(status);
	hapticsManager->OnHapticTick.AddRaw(this, &FHapticThread::GetCollision);
	hapticsManager->OneSecHapticTick.AddRaw(this, &FHapticThread::GetAnchor);
	hapticsManager->OnTexHapticTick.AddRaw(this, &FHapticThread::GetViscosity);



	while (UHapticThreadInput::getInst().shouldThreadRun()) {
		FVector position = haptico.getPosition();
		FMatrix rotation = haptico.getRotation();
		FVector linearVelocity = haptico.getLinearVelocity();
		FVector angularVelocity = haptico.getAngularVelocity();

		UHapticThreadOutput::getInst().setHapticCursorPosition(position);
		UHapticThreadOutput::getInst().setHapticCursorRotation(rotation);
		UHapticThreadOutput::getInst().setHapticCursorAngularVelocity(angularVelocity);
		UHapticThreadOutput::getInst().setHapticCursorLinearVelocity(linearVelocity);

		AHapticsManager* hapticManagerPointer = hapticsManager;
		hapticsManager->BHandler = &buttonHandler;

		bool button1clicked = buttonHandler.button1Clicked();
		bool button2clicked = buttonHandler.button2Clicked();
		AsyncTask(ENamedThreads::GameThread, [hapticManagerPointer, button1clicked, button2clicked, position, rotation]() {
			if (button1clicked) {
				hapticManagerPointer->button1Clicked();
			}
			else if (button2clicked) {
				hapticManagerPointer->button2Clicked();
			}

		});

		hapticsManager->broadCastNewHapticData(position, rotation, linearVelocity, angularVelocity);
		
		//FVector force = UHapticThreadInput::getInst().getForceToApply();
		FVector torque = UHapticThreadInput::getInst().getTorqueToApply();

		//UE_LOG(LogTemp, Warning, TEXT("appliedForce : %s"), *(appliedForce.ToString()));
		haptico.setForceAndTorque(appliedForce, torque);

	}
	haptico.disconnect();
	
}

void FHapticThread::GetViscosity(float v1)
{
	appliedForce = haptico.getLinearVelocity() * -v1;
	//printf("%f", -v1);
	//UE_LOG(LogTemp, Warning, TEXT("viscosity: %f"), -v1);
	//UE_LOG(LogTemp, Warning, TEXT("appliedForce : %s"), *(appliedForce.ToString()));


}

void FHapticThread::GetCollision(FVector BLocation, FVector HitLocation, FMatrix v2, FVector HitNormal, FVector RHitNormal)
{
	// if VDP is off
	if (HitNormal == FVector::ZeroVector)
	{
		appliedForce = FVector::ZeroVector;
		return; 

	}

	//Force Calculation
	float pDepth = FVector::DotProduct(HitNormal, (HitLocation - BLocation)); // brush to location
	float forceMag = FMath::Pow(2.0f, (pDepth + 2.5f));
	FVector damping = 0.6f * haptico.getLinearVelocity();
	if (forceMag > 0.0f)
	{
		appliedForce = FVector(RHitNormal * forceMag) - damping;
		//force = FVector(n * forceMag);

		appliedForce = FVector(FVector(-appliedForce.X, appliedForce.Y, appliedForce.Z));
		//UE_LOG(LogTemp, Warning, TEXT("f : %s"), *(appliedForce.ToString()));
	}
	else
	{
		appliedForce = FVector::ZeroVector;
	}
}

void FHapticThread::GetAnchor(FVector Blocation, FVector Anchor, FMatrix m1, FRotator direction, FVector v4)
{
	if (Anchor == FVector::ZeroVector)
	{
		appliedForce = FVector::ZeroVector;
		return;
	}

	FVector distance = direction.RotateVector(FVector(Blocation - Anchor)); // MC = C-M
	float distSize = distance.Size();


	float forceMag = 0.05 * distSize;
	distance.Normalize();
	FVector vel = haptico.getLinearVelocity();
	FVector damping = 0.5f * vel;
	//UE_LOG(LogTemp, Warning, TEXT("vel : %f"), vel.Size());
	if (distSize < 1.0 && vel.Size() < 0.1)
	{
		forceMag = 0.0f;
	}


	appliedForce = -FVector(-distance.X, distance.Y, distance.Z) * forceMag - damping;

}