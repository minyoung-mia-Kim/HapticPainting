// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlaneMesh.h"


// Sets default values
AProceduralPlaneMesh::AProceduralPlaneMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pm = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	pm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	SetRootComponent(pm);

	width = 2;
	height = 2;
	spacing = 10.0f;
	nGeneratedSection = 0;
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
void AProceduralPlaneMesh::Initialize(FVector position, FRotator rotation, FVector direction)
{
	//pm->SetWorldLocation(position);
	//pm->SetWorldRotation(rotation);
	UE_LOG(LogTemp, Warning, TEXT("creating on %d section"), nGeneratedSection);

	if (generateMesh)
	{
		generateMesh = false;

		ClearMeshData();

		GenerateVertices(position, rotation);
		GenerateTriangles();

		//Function that creates mesh section
		pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	}
	FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection);
	for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
		UE_LOG(LogTemp, Warning, TEXT("created X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X, 
																   ms->ProcVertexBuffer[i].Position.Y, 
																   ms->ProcVertexBuffer[i].Position.Z);
	prvHeight = height;
	prvWidth = width;
	nGeneratedSection++;
}
void AProceduralPlaneMesh::Initialize(FVector sPos, FVector ePos, FRotator rotation)
{
	pm->SetWorldLocation(sPos);
	pm->SetWorldRotation(rotation);
	width = FVector::Dist(sPos, ePos);

	if (generateMesh)
	{
		generateMesh = false;

		ClearMeshData();

		GenerateVertices();
		GenerateTriangles();

		//Function that creates mesh section
		pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	}
	FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection);
	for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
		UE_LOG(LogTemp, Warning, TEXT("created X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X, 
																ms->ProcVertexBuffer[i].Position.Y, 
																ms->ProcVertexBuffer[i].Position.Z);
	prvHeight = height;
	prvWidth = width;
	nGeneratedSection++;
}

void AProceduralPlaneMesh::Initialize(TArray<FVector> posArray, TArray<FRotator> rotArray)
{
	pm->SetWorldLocation(posArray[0]);
	pm->SetWorldRotation(rotArray[0]);

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

void AProceduralPlaneMesh::Update(FVector position, FRotator rotation)
{
	ClearMeshData();
	UE_LOG(LogTemp, Warning, TEXT("Working on %d section"), nGeneratedSection-1);

		FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection-1);
		FProcMeshVertex lastidx = ms->ProcVertexBuffer[ms->ProcVertexBuffer.Num() - 1];
		FVector prvVertex = lastidx.Position;
	float distance = FVector::Dist(pm->GetComponentLocation()+prvVertex, position);
	UE_LOG(LogTemp, Warning, TEXT("component X:%f, Y:%f, Z:%f"), pm->GetComponentLocation().X + prvVertex.X,
																pm->GetComponentLocation().Y + prvVertex.Y,
																pm->GetComponentLocation().Z + prvVertex.Z);
	UE_LOG(LogTemp, Warning, TEXT("update haptic position X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	UE_LOG(LogTemp, Warning, TEXT("distance : %f"), distance);
	height = FMath::Abs(distance) / spacing;
	if (height > 0)
	{
		float uvSpacing = 1.0f / FMath::Max(height, width);
		for (int32 y = 0; y < height; y++)
		{
			for (int32 x = 0; x < width; x++)
			{
				FVector WorldSpaceVertexLocation = position + GetTransform().TransformVector(FVector(prvVertex.X, prvVertex.Y + y * spacing, prvVertex.Z + x * spacing));
				//vertices.Add(FVector(x * spacing, y * spacing, 0.0f));
				vertices.Add(FVector(WorldSpaceVertexLocation));
				//vertices.Add(FVector(prvVertex.X, prvVertex.Y + y * spacing, prvVertex.Z + x * spacing));
				normals.Add(FVector(1.0f, 0.0f, 0.0f));
				uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
				vertexColors.Add(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
				tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			}
		}
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

		UE_LOG(LogTemp, Warning, TEXT("# of section : %d"), nGeneratedSection);
		pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
		ms = pm->GetProcMeshSection(nGeneratedSection);
		for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
			UE_LOG(LogTemp, Warning, TEXT("edited X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X, ms->ProcVertexBuffer[i].Position.Y, ms->ProcVertexBuffer[i].Position.Z);
		nGeneratedSection++;
	}
	distance = 0.0;
}

void AProceduralPlaneMesh::OnConstruction(const FTransform & Transform)
{
	//if (generateMesh)
	//{
	//	generateMesh = false;

	//	ClearMeshData();

	//	GenerateVertices();
	//	GenerateTriangles();

	//	//Function that creates mesh section
	//	pm->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	//}
}

void AProceduralPlaneMesh::GenerateVertices()
{
	float uvSpacing = 1.0f / FMath::Max(height, width);

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			vertices.Add(FVector(0.0f, y * spacing, x * spacing));
			normals.Add(FVector(-1.0f, 0.0f, 0.0f));
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
			vertexColors.Add(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
			tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		}
	}
}
void AProceduralPlaneMesh::GenerateVertices(FVector position, FRotator rotation)
{
	//width = FVector::Dist(posArray[0], posArray[posArray.Num()-1]);
	UE_LOG(LogTemp, Warning, TEXT("haptic position X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);

	float uvSpacing = 1.0f / FMath::Max(height, width);

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			FVector WorldSpaceVertexLocation = position + GetTransform().TransformVector(FVector(x * spacing, y * spacing, 0.0f));
			//vertices.Add(FVector(x * spacing, y * spacing, 0.0f));
			vertices.Add(FVector(WorldSpaceVertexLocation));
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

