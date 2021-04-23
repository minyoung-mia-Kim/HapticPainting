// Fill out your copyright notice in the Description page of Project Settings.

#include "PaintBrush.h"


// Sets default values
APaintBrush::APaintBrush()
{
	RComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RComponent);

	cursor = CreateDefaultSubobject<USphereComponent>(TEXT("Cursor"));
	cursor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	cursor->SetNotifyRigidBodyCollision(true);
	cursor->SetWorldScale3D(FVector(0.05f, 0.1f, 0.1f));
	//cursor->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	cursor->bHiddenInGame = false;
	//SetRootComponent(cursor);

	brush = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Brush"));
	brush->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	brush->SetWorldLocation(FVector(cursor->GetScaledSphereRadius(), 0.0f, 0.0f));
	brush->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	CreateBrushCursor(10.0f, FLinearColor::White, "Material'/Game/HDAssets/M_grunge1.M_grunge1'");

}

// Called when the game starts or when spawned
void APaintBrush::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APaintBrush::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/*
	Redraw the cursor by the new brush information
*/

void APaintBrush::RefreshBrushCursor(float bSize, FLinearColor brushColor, float viscosity, FString tex)
{
	brush->ClearAllMeshSections();
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushColor.ToString()));
	//this->viscosity = viscosity;
	CreateBrushCursor(bSize, brushColor, tex);

}

void APaintBrush::CreateBrushCursor(float bSize, FLinearColor brushColor, FString tex)
{
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	int32 height = 2;
	int32 width = 2;
	brushSize = bSize;
	float uvSpacing = 1.0f / FMath::Max(height, width);

	Material = LoadObject<UMaterialInterface>(nullptr, *tex);

	vertices.Add(FVector(0.0f, 1.0f, brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, 1.0f, -brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, -brushSize / 2));
	vertexColors.Add(brushColor); //red


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
	//Opposite Front-face
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + 1);				//current vertex + one right		: 1
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3

			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3
			triangles.Add(x + (y * width) + width);			//current vertex + row				: 2
		}
	}
	brush->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	brush->SetMaterial(0, Material);

}

