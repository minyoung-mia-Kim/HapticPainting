// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlaneMesh.h"


// Sets default values
AProceduralPlaneMesh::AProceduralPlaneMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pm = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	pm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	width = 2;
	height = 2;
	spacing = 10.0f;

	generateMesh = true;
}


// Called when the game starts or when spawned
void AProceduralPlaneMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralPlaneMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Initialize the mesh's loction
void AProceduralPlaneMesh::Initialize(FVector position, FRotator rotation)
{
	pm->SetWorldLocation(position);
	pm->SetWorldRotation(rotation);
}
void AProceduralPlaneMesh::OnConstruction(const FTransform & Transform)
{
	if (generateMesh)
	{
		generateMesh = false;

		ClearMeshData();

		GenerateVertices();
		GenerateTriangles();

		//Function that creates mesh section
		pm->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	}
}

void AProceduralPlaneMesh::GenerateVertices()
{
	float uvSpacing = 1.0f / FMath::Max(height, width);

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			vertices.Add(FVector(x * spacing, y * spacing, 0.0f));
			normals.Add(FVector(0.0f, 0.0f, 1.0f));
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
			vertexColors.Add(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
			tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		}
	}
}

// Assemble the individual triangles
void AProceduralPlaneMesh::GenerateTriangles()
{
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
}

void AProceduralPlaneMesh::ClearMeshData()
{
	vertices.Empty();
	triangles.Empty();
	uvs.Empty();
	normals.Empty();
	vertexColors.Empty();
	tangents.Empty();
}

