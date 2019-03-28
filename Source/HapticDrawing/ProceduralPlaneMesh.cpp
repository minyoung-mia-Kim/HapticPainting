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
	//UMaterialInstanceDynamic * InstanceMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	FString MaterialAddress = "Material'/Game/ArchVis/Materials/M_Carptet_Mat.M_Carptet_Mat'";
	Material = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/M_Color.M_Color'"));
	pm->SetMaterial(0, Material);

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
		pm->SetMaterial(nGeneratedSection, Material);

	}
	FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection);
	for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
		UE_LOG(LogTemp, Warning, TEXT("created X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X,
			ms->ProcVertexBuffer[i].Position.Y,
			ms->ProcVertexBuffer[i].Position.Z);
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
	UE_LOG(LogTemp, Warning, TEXT("Working on idx %d section"), nGeneratedSection);

	/* Bring the vertice information from the right before section*/
	FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection - 1);
	TArray<FProcMeshVertex> prvVertex = ms->ProcVertexBuffer;

	/* Mesh component's coordination is always 0, 0, 0 since its not moving */
	/*UE_LOG(LogTemp, Warning, TEXT("component X:%f, Y:%f, Z:%f"), pm->GetComponentLocation().X + prvVertex[3].Position.X,
		pm->GetComponentLocation().Y + prvVertex[3].Position.Y,
		pm->GetComponentLocation().Z + prvVertex[3].Position.Z);*/

	UE_LOG(LogTemp, Warning, TEXT("update haptic position X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	float uvSpacing = 1.0f / FMath::Max(height, width);
	
	// The distance between this mesh component and the haptic position
	// Actually, it's the same with the haptic position itself since the mesh component coordination is (0,0,0).
	FVector dis = GetTransform().TransformVector(position);
	UE_LOG(LogTemp, Warning, TEXT("Distance X:%f, Y:%f, Z:%f"), dis.X, dis.Y, dis.Z);

	UE_LOG(LogTemp, Warning, TEXT("vertex1 X:%f, Y:%f, Z:%f"), prvVertex[2].Position.X, prvVertex[2].Position.Y, prvVertex[2].Position.Z);
	vertices.Add(FVector(prvVertex[2].Position));
	vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	UE_LOG(LogTemp, Warning, TEXT("vertex2 X:%f, Y:%f, Z:%f"), prvVertex[3].Position.X, prvVertex[3].Position.Y, prvVertex[3].Position.Z);
	vertices.Add(FVector(prvVertex[3].Position));
	vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green

	UE_LOG(LogTemp, Warning, TEXT("vertex3 X:%f, Y:%f, Z:%f"), position.X, position.Y + spacing, position.Z + spacing);
	vertices.Add(FVector(position.X, position.Y + spacing, position.Z + spacing));
	vertexColors.Add(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));//blue

	UE_LOG(LogTemp, Warning, TEXT("vertex4 X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z + spacing);
	vertices.Add(FVector(position.X, position.Y, position.Z + spacing));
	vertexColors.Add(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //white

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			//vertices.Add(FVector(prvVertex.X, prvVertex.Y + y * spacing, prvVertex.Z + x * spacing));
			normals.Add(FVector(1.0f, 0.0f, 0.0f));
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
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

	pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	pm->SetMaterial(nGeneratedSection, Material);

	ms = pm->GetProcMeshSection(nGeneratedSection);
	//for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
		//UE_LOG(LogTemp, Warning, TEXT("edited X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X, ms->ProcVertexBuffer[i].Position.Y, ms->ProcVertexBuffer[i].Position.Z);
	nGeneratedSection++;
	UE_LOG(LogTemp, Warning, TEXT("Total # of section : %d"), nGeneratedSection+1);

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
			normals.Add(FVector(1.0f, 0.0f, 0.0f));
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
	vertices.Add(FVector(position + GetTransform().TransformVector(FVector(0.0f, 0.0f, 0.0f))));
	vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position + GetTransform().TransformVector(FVector(0.0f, 10.0f, 0.0f))));
	vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green

	vertices.Add(FVector(position + GetTransform().TransformVector(FVector(0.0f, 10.0f, 10.0f))));
	vertexColors.Add(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); //blue

	vertices.Add(FVector(position + GetTransform().TransformVector(FVector(0.0f, 0.0f, 10.0f))));
	vertexColors.Add(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //white

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			//vertices.Add(FVector(x * spacing, y * spacing, 0.0f));
			normals.Add(FVector(1.0f, 0.0f, 0.0f));
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
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

