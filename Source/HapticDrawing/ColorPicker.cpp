// Fill out your copyright notice in the Description page of Project Settings.

#include "ColorPicker.h"


// Sets default values
AColorPicker::AColorPicker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ColorPickerCanvus = CreateDefaultSubobject<UBoxComponent>(TEXT("Plane"));
	//Plane->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	ColorPickerCanvus->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.01f));
	ColorPickerCanvus->ShapeColor = FColor::Black;
	ColorPickerCanvus->bVisible = true;
	SetRootComponent(ColorPickerCanvus);
	
	Hue = FLinearColor(1.0f, 0.0f, 0.0f);
	SelectedColor = Hue;

}


// Called when the game starts or when spawned
void AColorPicker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AColorPicker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

