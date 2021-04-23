// Fill out your copyright notice in the Description page of Project Settings.

#include "PainterPawn.h"
#include "Components/ShapeComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"


// Sets default values
APainterPawn::APainterPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*SetRootComponent((USceneComponent*)GetCollisionComponent());
	SetActorEnableCollision(false);
	GetRootComponent()->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));*/
	RComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RComponent);
	
	SPArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SPArm->AttachToComponent(RComponent, FAttachmentTransformRules::KeepRelativeTransform);

	VRcamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PawnCamera"));

	MC_Left = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MC_Left"));
	MC_Left->Hand = EControllerHand::Left;
	MC_Left->SetupAttachment(RComponent);

	RMComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RMComponent"));
	RMComponent->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	
	MC_Right = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MC_Right"));
	MC_Right->Hand = EControllerHand::Right;
	MC_Right->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void APainterPawn::BeginPlay()
{
	Super::BeginPlay();
	//EnableInput(GetWorld()->GetFirstPlayerController());
	//Cpicker = Cast<AColorPicker>(MC_Left->GetChildComponent(0));
	Cpicker->FSelelctedBrushUpdateDelegate.AddDynamic(this, &APainterPawn::Color);
	Cpicker->FTextureDelegate.AddDynamic(this, &APainterPawn::Texture);
}

// Called every frame
void APainterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//AddMovementInput(GetActorForwardVector() * MovementVector.Y + GetActorRightVector() * MovementVector.X, 1);
}

// Called to bind functionality to input
void APainterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//Motion controller bindings
	/* Left MC*/
	InputComponent->BindAxis("MoveForward", this, &APainterPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APainterPawn::MoveRight);
	InputComponent->BindAction("VDP", IE_Pressed, this, &APainterPawn::ActivateVDP);

	/* Right MC*/
	// Draw_RightTrigger in Blueprint


}

// Left Trigger Press
float APainterPawn::TurnLeft()
{
	float minus = -1;
	return minus;
}

// Left Trigger Release
float APainterPawn::TurnRight()
{
	float plus = 1;
	return plus;
}



/**
 * function: getHMDType
 * This method communicates with the Unreal Engine to determine what type of HMD
 * is attached to host device.
 *
 * @return FString    name of the HMD device; "None" if there is no HMD
 */
//FString APainterPawn::getHMDType() {
//	IHeadMountedDisplay* hmd = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());
//	if (hmd) {
//		EHMDDeviceType::Type current_hmd = hmd->GetHMDDeviceType();
//
//		switch (current_hmd) {
//		case EHMDDeviceType::DT_ES2GenericStereoMesh:
//			return FString("Generic");
//			break;
//		case EHMDDeviceType::DT_GearVR:
//			return FString("GearVR");
//			break;
//		case EHMDDeviceType::DT_Morpheus:
//			return FString("PSVR");
//			break;
//		case EHMDDeviceType::DT_OculusRift:
//			return FString("OculusRift");
//			break;
//		case EHMDDeviceType::DT_SteamVR:
//			return FString("SteamVR");
//			break;
//		default:
//			return FString("None");
//			break;
//		}
//	}
//
//	return FString("None");
//}

void APainterPawn::MoveRight(float Val)
{


	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it and adjust a speed
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val*0.1);
		}
	}

}

void APainterPawn::Color(FLinearColor sColor, float sSize)
{
	FSelectedBrushUpdateDelegate.Broadcast(sColor, sSize);
}

void APainterPawn::Texture(int tex)
{
	FTexDelegate.Broadcast(tex);
}

void APainterPawn::ActivateVDP()
{
	FActivateVDPDelegate.Broadcast();

}

void APainterPawn::MoveForward(float Val)
{

	if (Val != 0.f)
	{
		if (Controller)
		{
			FRotator const ControlSpaceRot = Controller->GetControlRotation();

			// transform to world space and add it and adjust a speed
			AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val*0.1);
		}
	}
}

void APainterPawn::MoveUp_World(float Val)
{
	if (Val != 0.f)
	{
		//Adjust a speed
		AddMovementInput(FVector::UpVector, Val*0.1);

	}
}

void APainterPawn::GetHapticCursor(FVector Pos)
{
	if (Pos != PrevCursorPos)
	{
		RotationAngle = FMath::Acos(FVector::DotProduct(FVector(MC_Left->GetComponentLocation() - PrevCursorPos), FVector(MC_Left->GetComponentLocation() - Pos)));
	}
	else
	{
		RotationAngle = 0.0f;
	}
	PrevCursorPos = Pos;

}

//void APainterPawn::TurnAtRate(float Rate)
//{
//	// calculate delta for this frame from the rate information
//	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
//}
//
//void APainterPawn::LookUpAtRate(float Rate)
//{
//	// calculate delta for this frame from the rate information
//	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
//}

