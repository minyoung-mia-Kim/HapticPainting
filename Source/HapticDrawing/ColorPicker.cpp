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
	//ColorPalette = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ColorPalette"));
	//CreateColorPalette();
	//ColorPalette->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//ColorPalette->SetRelativeLocation(FVector(-0.3f, 0.3f, 10.0f));

}

void AColorPicker::CreateColorPalette()
{
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	int32 height = 20;
	int32 width = 20;
	float spacing = 20.f;
	float uvSpacing = 1.0f / FMath::Max(height, width);

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/M_NonLight.M_NonLight'"));
	vertices.Add(FVector(spacing / 2, spacing / 2, 0.0f));
	vertexColors.Add(Hue); //red

	vertices.Add(FVector(-spacing / 2, spacing / 2, 0.0f));
	vertexColors.Add(FLinearColor::Black); //red

	vertices.Add(FVector(spacing / 2, -spacing / 2, 0.0f));
	vertexColors.Add(FLinearColor::White); //red

	vertices.Add(FVector(-spacing / 2, -spacing / 2, 0.0f));
	vertexColors.Add(FLinearColor::Black); //red

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
			tangents.Add(FProcMeshTangent(0.0f, 1.0f, 0.0f));
		}
	}

	//Front-face
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Add(x + (y * width));					//current vertex
			triangles.Add(x + (y * width) + width);			//current vertex + row
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right

			triangles.Add(x + (y * width));					//current vertex
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right
			triangles.Add(x + (y * width) + 1);				//current vertex + one right
		}
	}

	ColorPalette->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	ColorPalette->SetMaterial(0, Material);
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

