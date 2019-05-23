// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlaneMesh.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"


// Sets default values
AProceduralPlaneMesh::AProceduralPlaneMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//SetActorEnableCollision(true);
	pm = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	pm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	//pm->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pm->SetEnableGravity(false);
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
//void AProceduralPlaneMesh::Initialize(FVector position, FRotator rotation, FVector direction)
//{
//	//pm->SetWorldLocation(position);
//	//pm->SetWorldRotation(rotation);
//	UE_LOG(LogTemp, Warning, TEXT("creating on %d section"), nGeneratedSection);
//
//	if (generateMesh)
//	{
//		generateMesh = false;
//
//		ClearMeshData();
//
//		GenerateVertices(position, rotation);
//		GenerateTriangles();
//
//		//Function that creates mesh section
//		pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
//		pm->SetMaterial(nGeneratedSection, Material);
//
//	}
//	FProcMeshSection* ms = pm->GetProcMeshSection(nGeneratedSection);
//	for (int i = 0; i < ms->ProcVertexBuffer.Num(); i++)
//		UE_LOG(LogTemp, Warning, TEXT("created X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[i].Position.X,
//			ms->ProcVertexBuffer[i].Position.Y,
//			ms->ProcVertexBuffer[i].Position.Z);
//	nGeneratedSection++;
//}

void AProceduralPlaneMesh::Initialize(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FString mode)
{
	ClearMeshData();
	float uvSpacing = 1.0f / FMath::Max(height, width);

	//	Set material
	Material = LoadObject<UMaterialInterface>(nullptr, *mode);
	pm->SetMaterial(0, Material);

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)))));
	//vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)))));
	//vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green
	uvs.Add(FVector2D(0, 0));
	uvs.Add(FVector2D(1, 0));
	prvPos = position;

}
/* // Old function
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
	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 5.0f, 5.0f)))));
	vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, -5.0f, 5.0f)))));
	vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 5.0f, -5.0f)))));
	vertexColors.Add(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); //blue

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, -5.0f, -5.0f)))));
	vertexColors.Add(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //white


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			normals.Add(FVector(-1.0f, 0.0f, 0.0f));
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
			tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		}
	}
}
*/
// Assemble the individual triangles
void AProceduralPlaneMesh::GenerateTriangles()
{
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + width);			//current vertex + row				: 2
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3

			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3
			triangles.Add(x + (y * width) + 1);				//current vertex + one right		: 1
		}
	}
}
//Assemble the front or back side trangles
void AProceduralPlaneMesh::GenerateOppositeTriangles()
{
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
}


void AProceduralPlaneMesh::Update(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color)
{
	//UE_LOG(LogTemp, Warning, TEXT("Working on idx %d section"), nGeneratedSection);

	/* Mesh component's coordination is always 0, 0, 0 since its not moving */

	float uvSpacing = 1.0f / FMath::Max(nGeneratedSection, width);


	// The distance between this mesh component and the haptic position
	// Actually, it's the same with the haptic position itself since the mesh component coordination is (0,0,0).
	FVector dis = GetTransform().TransformVector(position);


	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	//vertexColors.Add(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); //blue

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));
	//vertexColors.Add(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //white


	/* Normal and Tangent */
	//Normal 1 : Mesh front - Forward
	//FVector Normal = FVector::CrossProduct(FVector(vertices[2] - vertices[3]), FVector(vertices[1] - vertices[3]));

	//Normal 2 : Hapatic - Forward
	FVector Normal = rotation.Vector();
	Normal = -Normal;
	Normal.Normalize();



	FVector surfaceTangent = vertices[2] - vertices[3]; //p1 to p3 being FVectors
	//	FVector::GetSafeNormal:
	//	Gets a normalized copy of the vector, checking it is safe to do so based on the length. 
	//	Returns zero vector if vector length is too small to safely normalize.
	surfaceTangent = surfaceTangent.GetSafeNormal();

	if (surfaceTangent == FVector().ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("vector length is too small to safely normalize"));

	/* Debug */
	//UE_LOG(LogTemp, Warning, TEXT("update haptic position X:%f, Y:%f, Z:%f"), position.X, position.Y, position.Z);
	//UE_LOG(LogTemp, Warning, TEXT("selected Color: %s"), *(color.ToString()));
	//UE_LOG(LogTemp, Warning, TEXT("Distance X:%f, Y:%f, Z:%f"), dis.X, dis.Y, dis.Z);
	//UE_LOG(LogTemp, Warning, TEXT("vertex3 X:%f, Y:%f, Z:%f"), vertices[2].X, vertices[2].Y, vertices[2].Z);
	//UE_LOG(LogTemp, Warning, TEXT("vertex4 X:%f, Y:%f, Z:%f"), vertices[3].X, vertices[3].Y, vertices[3].Z);
	//UE_LOG(LogTemp, Warning, TEXT("Normal X:%f, Y:%f, Z:%f"), Normal.X, Normal.Y, Normal.Z);
	DrawDebugLine(GetWorld(), position, position + Normal * 5.0f, FColor::Red, true, 0, 0, 0.2);
	DrawDebugLine(GetWorld(), position, position + surfaceTangent * 5.0f, FColor::Blue, true, 0, 0, 0.2);


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			normals.Add(Normal);
			vertexColors.Add(color);
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
		}
	}
	GenerateTriangles();
	//	Two-sided front-faces
	GenerateOppositeTriangles();

	/* Add a mesh section */
	pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	//pm->bUseComplexAsSimpleCollision = false;
	pm->SetCollisionConvexMeshes({ vertices });


	/*	Get the first material of the static mesh and turn it into a material instance */
	UMaterialInstanceDynamic* DynamicMatInstance = pm->CreateAndSetMaterialInstanceDynamic(0);

	//If we have a valid dynamic material instance, modify its parameters
	if (DynamicMatInstance)
	{
		float ex;
		DynamicMatInstance->GetScalarParameterValue(FName("UTiling"), ex);
		DynamicMatInstance->SetScalarParameterValue(FName("UTiling"), ex - 0.001);
	}
	pm->SetMaterial(nGeneratedSection, Material);


	/* increase section idx */
	nGeneratedSection++;

	/* Store mesh's location */
	centerPos.Add(FVector((prvPos + position)/2));
	centerNormals.Add(Normal);
	prvPos = position;

	/* Clean mesh data for next mesh section*/
	ClearMeshData();

	/* For the next mesh section */
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	//vertexColors.Add(color); //white
	//vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));
	//vertexColors.Add(color); //white
	//vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green

	/* Debug */
	//UE_LOG(LogTemp, Warning, TEXT("vertex1 X:%f, Y:%f, Z:%f"), vertices[0].X, vertices[0].Y, vertices[0].Z);
	//UE_LOG(LogTemp, Warning, TEXT("vertex2 X:%f, Y:%f, Z:%f"), vertices[1].X, vertices[1].Y, vertices[1].Z);

	/* UVs */
	UE_LOG(LogTemp, Warning, TEXT("###: %d"), pm->GetNumSections());
	float NSection = float(pm->GetNumSections());
	float newV;
	float newV1;

	for (int i = pm->GetNumSections()-1; i > 0; i--)
	{
		if (i != 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("i = %d, N= %f"), i, NSection);
			float n = float(i + 1);
			newV1 = (n-1) * 100 / NSection;
			pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, newV1 / 100);
			pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, newV1 / 100);

			newV = n*100 / NSection;
			pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, newV / 100);
			pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, newV / 100);

			/* Debug */
			//UE_LOG(LogTemp, Warning, TEXT("0000 %f : %f"), n, newV/100);
			//UE_LOG(LogTemp, Warning, TEXT("2222 %f : %f"), n-1, newV1/100);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("v: %f"), FMath::Sin(90 / i + 1)));
			//UE_LOG(LogTemp, Warning, TEXT("uv3 U:%f, V:%f"), pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0.X, pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0.Y);

		}
		else
		{
			pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, FMath::Sin(0));
			pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, FMath::Sin(0));
		}
	}
}

void AProceduralPlaneMesh::OnConstruction(const FTransform & Transform)
{
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

