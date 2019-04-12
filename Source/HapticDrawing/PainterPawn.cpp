// Fill out your copyright notice in the Description page of Project Settings.

#include "PainterPawn.h"
#include "Components/ShapeComponent.h"


// Sets default values
APainterPawn::APainterPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent((USceneComponent*)GetCollisionComponent());
	SetActorEnableCollision(false);
	GetRootComponent()->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
}

// Called when the game starts or when spawned
void APainterPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APainterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APainterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


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

void APainterPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void APainterPawn::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

