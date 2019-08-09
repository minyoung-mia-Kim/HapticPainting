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
	haptico.connect();
	hapticsManager->OnHapticTick.AddRaw(this, &FHapticThread::test);

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
		FVector force = UHapticThreadInput::getInst().getForceToApply();
		FVector torque = UHapticThreadInput::getInst().getTorqueToApply();
		haptico.setForceAndTorque(force, torque);

	}
	haptico.disconnect();
	
}

void FHapticThread::test(FVector v1, FMatrix v2, FVector v3, FVector v4)
{
		UE_LOG(LogTemp, Warning, TEXT("v1 : %s"), *(v1.ToString()));
}