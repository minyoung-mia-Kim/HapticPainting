// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlaneMesh.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"


// Sets default values
AProceduralPlaneMesh::AProceduralPlaneMesh()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//SetActorEnableCollision(true);
	pm = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	pm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	//pm->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	pm->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	pm->SetEnableGravity(false);
	SetRootComponent(pm);

	width = 2;
	height = 2;
	spacing = 10.0f;
	nGeneratedSection = 0;
	generateMesh = true;
	/* Clean mesh data for next mesh section*/
	ClearMeshData();
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

	//UE_LOG(LogTemp, Warning, TEXT("??????"));

	/*Save mesh section data*/
	TotalVertice.Append(vertices);

	/*material saveeee*/
	ArrMeshesections.color = color;
	ArrMeshesections.Material = mode;
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
	//FVector dis = GetTransform().TransformVector(position);


	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	//vertexColors.Add(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); //blue

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));
	//vertexColors.Add(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //white


	/* Normal and Tangent */
	//Normal 1 : Mesh front - Forward
	FVector Normal = FVector::CrossProduct(FVector(vertices[1] - vertices[3]), FVector(vertices[2] - vertices[3])); //31, 32
	Normal.Normalize();

	//Normal 2 : Hapatic - Forward
	FVector HapticOrientation = -rotation.Vector();
	//Normal = -Normal;

	if (FVector::DotProduct(Normal, HapticOrientation) < 0.0)
		Normal = -Normal;


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
	//UE_LOG(LogTemp, Warning, TEXT("Normal %s"), *(Normal.ToString()));
	//DrawDebugLine(GetWorld(), position, position + Normal * 5.0f, FColor::Red, true, 0, 0, 0.2);
	//DrawDebugLine(GetWorld(), position, position + surfaceTangent * 5.0f, FColor::Blue, true, 0, 0, 0.2);


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			normals.Add(Normal); // 4
			vertexColors.Add(color);
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
		}
	}
	//	Two-sided front-faces
	GenerateTriangles();
	GenerateOppositeTriangles();

	/* Add a mesh section */
	pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	//pm->bUseComplexAsSimpleCollision = false;
	pm->SetCollisionConvexMeshes({ vertices });

	/* UVs */
	//UE_LOG(LogTemp, Warning, TEXT("###: %d"), pm->GetNumSections());
	float NSection = float(pm->GetNumSections());
	float newV;
	float newV1;

	for (int i = pm->GetNumSections() - 1; i > 0; i--)
	{
		if (i != 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("i = %d, N= %f"), i, NSection);
			/* Multiple 100 to preserve? float point */
			float n = float(i + 1);
			newV1 = (n - 1) * 100 / NSection;
			pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, newV1 / 100);
			pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, newV1 / 100);

			newV = n * 100 / NSection;
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




	///*	Get the first material of the static mesh and turn it into a material instance */
	//UMaterialInstanceDynamic* DynamicMatInstance = pm->CreateAndSetMaterialInstanceDynamic(0);

	////If we have a valid dynamic material instance, modify its parameters
	//if (DynamicMatInstance)
	//{
	//	float ex;
	//	DynamicMatInstance->GetScalarParameterValue(FName("UTiling"), ex);
	//	DynamicMatInstance->SetScalarParameterValue(FName("UTiling"), ex - 0.001);
	//}


	pm->SetMaterial(nGeneratedSection, Material);


	/* Store mesh's location */
	centerPos.Add(FVector((prvPos + position) / 2));
	centerNormals.Add(Normal);
	prvPos = position;

	TotalNormal.Add(normals[0]);
	TotalNormal.Add(normals[1]);


	/* increase section idx */
	nGeneratedSection++;

	/* Clean mesh data for next mesh section*/
	ClearMeshData();

	/* For the next mesh section */
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	//vertexColors.Add(color); //white
	//vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));
	//vertexColors.Add(color); //white
	//vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green

	/*Save mesh section data*/
	TotalVertice.Append(vertices);

	/* Debug */
	//UE_LOG(LogTemp, Warning, TEXT("vertex1 X:%f, Y:%f, Z:%f"), vertices[0].X, vertices[0].Y, vertices[0].Z);
	//UE_LOG(LogTemp, Warning, TEXT("vertex2 X:%f, Y:%f, Z:%f"), vertices[1].X, vertices[1].Y, vertices[1].Z);
	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %d "), TotalVertice.Num());

}

void AProceduralPlaneMesh::MergeSections()
{
	if (bMerged)
		return;

	int sectionNum = 0;
	ClearMeshData();
	pm->ClearAllMeshSections();
	FVector Normal;
	FVector surfaceTangent;
	for (int i = 0; i < TotalVertice.Num(); i += 2)
	{
		if (i + 2 < TotalVertice.Num())
		{
			sectionNum++;
			//Normal = FVector::CrossProduct(FVector(TotalVertice[i + 1] - TotalVertice[i + 3]), FVector(TotalVertice[i + 2] - TotalVertice[i + 3])); //31, 32
			//Normal.Normalize();

			surfaceTangent = TotalVertice[i + 2] - TotalVertice[i + 3]; //p1 to p3 being FVectors
			surfaceTangent = surfaceTangent.GetSafeNormal();

			//DrawDebugLine(GetWorld(), TotalVertice[i + 2], TotalVertice[i + 2] + TotalNormal[i] * 5.0f, FColor::Red, true, 0, 0, 0.2);
			//DrawDebugLine(GetWorld(), TotalVertice[i + 2], TotalVertice[i + 2] + surfaceTangent * 5.0f, FColor::Blue, true, 0, 0, 0.2);

			for (int32 y = 0; y < height; y++)
			{
				vertexColors.Add(ArrMeshesections.color);
				//normals.Add(Normal);
				tangents.Add(FProcMeshTangent(surfaceTangent, true));

			}
			for (int32 y = 0; y < height - 1; y++)
			{
				for (int32 x = 0; x < width - 1; x++)
				{
					//Frontface
					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width);			//current vertex + row				: 2
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3

					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3
					triangles.Add(i + x + (y * width) + 1);				//current vertex + one right		: 1

					//Backface
					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + 1);				//current vertex + one right		: 1
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3

					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3
					triangles.Add(i + x + (y * width) + width);			//current vertex + row				: 2
				}
			}
		}
		else
		{
			vertexColors.Add(ArrMeshesections.color);
			vertexColors.Add(ArrMeshesections.color);
			//normals.Add(Normal);
			//normals.Add(Normal);
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
			tangents.Add(FProcMeshTangent(surfaceTangent, true));

		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("%d"), sectionNum);

	//UVS
	float NSection = float(sectionNum);
	float newV1;

	//for (int i = sectionNum - 2; i > 0; i--)
	for (int i = 0; i < TotalVertice.Num() / 2; i++)
	{

		float n = float(i);
		newV1 = n * 100 / NSection;
		uvs.Add(FVector2D(0, newV1 / 100));
		uvs.Add(FVector2D(1, newV1 / 100));

	}

	TotalNormal.Add(FVector(TotalNormal.Last()));
	TotalNormal.Add(FVector(TotalNormal.Last()));


	///*Check whether each feature has the same number of data with its vertices */
	//UE_LOG(LogTemp, Warning, TEXT("TotalVertice %d"), TotalVertice.Num());
	//UE_LOG(LogTemp, Warning, TEXT("TotalNormal %d"), TotalNormal.Num());
	//UE_LOG(LogTemp, Warning, TEXT("uvs %d"), uvs.Num());
	//UE_LOG(LogTemp, Warning, TEXT("vertexColors %d"), vertexColors.Num());
	//UE_LOG(LogTemp, Warning, TEXT("tangents %d"), tangents.Num());



	pm->CreateMeshSection_LinearColor(0, TotalVertice, triangles, TotalNormal, uvs, vertexColors, tangents, true);
	FString m = ArrMeshesections.Material;
	Material = LoadObject<UMaterialInterface>(nullptr, *m);
	pm->SetMaterial(0, Material);
	pm->SetCollisionConvexMeshes({ vertices });
	bMerged = true;
	//UE_LOG(LogTemp, Warning, TEXT("color %s"), *(pm->GetProcMeshSection(0)->ProcVertexBuffer[2].Color.ToString()));
	//UE_LOG(LogTemp, Warning, TEXT("normals %s"), *(pm->GetProcMeshSection(0)->ProcVertexBuffer[2].Normal.ToString()));


	/*
		for (int i=0;i<centerPos.Num(); i++)
		{
			DrawDebugPoint(GetWorld(), centerPos[i], 5.5f, FColor::Black, true, 0, 0);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *centerPos[i].ToString());

		}*/
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

void AProceduralPlaneMesh::SetHidden(bool bHidden)
{
	/*pm->SetVisibility(bVisible);*/
	this->SetActorHiddenInGame(bHidden);
}



void AProceduralPlaneMesh::ActorSaveDataLoaded_Implementation()
{
}

void AProceduralPlaneMesh::ActorSaveDataSaved_Implementation()
{
}

FMeshSectionData AProceduralPlaneMesh::getAllMeshsections()
{
	ArrMeshesections.vertices = TotalVertice;
	ArrMeshesections.normals = TotalNormal;
	ArrMeshesections.centerPosition = centerPos;
	ArrMeshesections.centerNormal = centerNormals;

	return ArrMeshesections;
}


void AProceduralPlaneMesh::LoadMeshsections(FMeshSectionData msData)
{
	/*Several mesh sections*/
	/////////////////////////////////////////////
	//int nGeneratedSection = 0;
	//for (int i = 0; i < msData.vertices.Num(); i += 2)
	//{
	//	if (i + 2 < msData.vertices.Num())
	//	{
	//		vertices.Add(msData.vertices[i]);
	//		vertices.Add(msData.vertices[i + 1]);
	//		vertices.Add(msData.vertices[i + 2]);
	//		vertices.Add(msData.vertices[i + 3]);

	//		FVector Normal = FVector::CrossProduct(FVector(vertices[1] - vertices[3]), FVector(vertices[2] - vertices[3])); //31, 32
	//		Normal.Normalize();
	//		FVector surfaceTangent = vertices[2] - vertices[3]; //p1 to p3 being FVectors
	//		surfaceTangent = surfaceTangent.GetSafeNormal();
	//		for (int32 y = 0; y < height; y++)
	//		{
	//			for (int32 x = 0; x < width; x++)
	//			{
	//				normals.Add(Normal);
	//				vertexColors.Add(msData.color);
	//				tangents.Add(FProcMeshTangent(surfaceTangent, true));
	//			}
	//		}
	//	}

	//	GenerateTriangles();
	//	GenerateOppositeTriangles();

	//	pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	//	FString m = msData.Material;
	//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *m);

	//	Material = LoadObject<UMaterialInterface>(nullptr, *m);
	//	pm->SetMaterial(nGeneratedSection, Material);

	//	nGeneratedSection++;
	//	pm->SetCollisionConvexMeshes({ vertices });

	//	ClearMeshData();
	//}



	//////////////////////////////////////////////////////////////////////////////////
	/* One mesh section */
	vertices = msData.vertices;
	normals = msData.normals;

	int sectionNum = 0;
	FVector surfaceTangent;
	for (int i = 0; i < msData.vertices.Num(); i += 2)
	{
		if (i + 2 < msData.vertices.Num())
		{
			sectionNum++;
			//FVector Normal = FVector::CrossProduct(FVector(vertices[i + 1] - vertices[i + 3]), FVector(vertices[i + 2] - vertices[i + 3])); //31, 32
			//Normal.Normalize();

			surfaceTangent = vertices[i + 2] - vertices[i + 3]; //p1 to p3 being FVectors
			surfaceTangent = surfaceTangent.GetSafeNormal();
			//DrawDebugLine(GetWorld(), vertices[i + 2], vertices[i + 2] + surfaceTangent * 5.0f, FColor::Blue, true, 0, 0, 0.2);
			//DrawDebugLine(GetWorld(), vertices[i + 2], vertices[i + 2] + Normal * 5.0f, FColor::Red, true, 0, 0, 0.2);


			for (int32 x = 0; x < width; x++)
			{
				vertexColors.Add(msData.color);
				//normals.Add(Normal);
				tangents.Add(FProcMeshTangent(surfaceTangent, true));
			}


			for (int32 y = 0; y < height - 1; y++)
			{
				for (int32 x = 0; x < width - 1; x++)
				{
					//Frontface
					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width);			//current vertex + row				: 2
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3

					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3
					triangles.Add(i + x + (y * width) + 1);				//current vertex + one right		: 1

					//Backface
					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + 1);				//current vertex + one right		: 1
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3

					triangles.Add(i + x + (y * width));					//current vertex					: 0
					triangles.Add(i + x + (y * width) + width + 1);		//current vertex + row + one right	: 3
					triangles.Add(i + x + (y * width) + width);			//current vertex + row				: 2
				}
			}
		}
	}
	vertexColors.Add(msData.color);
	vertexColors.Add(msData.color);
	tangents.Add(FProcMeshTangent(surfaceTangent, true));
	tangents.Add(FProcMeshTangent(surfaceTangent, true));

	//UVS
	float NSection = float(sectionNum);
	//	float newV;
	float newV1;

	//for (int i = sectionNum - 2; i > 0; i--)
	for (int i = 0; i < msData.vertices.Num() / 2; i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%d"), i);

		//if (i != 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("i = %d, N= %f"), i, NSection);
			/* Multiple 100 to preserve? float point */
			//float n = float(i + 1);
			//newV1 = (n - 1) * 100 / NSection;
			//pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, newV1 / 100);
			//pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, newV1 / 100);

			//newV = n * 100 / NSection;
			//pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, newV / 100);
			//pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, newV / 100);

			float n = float(i);
			newV1 = n * 100 / NSection;
			uvs.Add(FVector2D(0, newV1 / 100));
			uvs.Add(FVector2D(1, newV1 / 100));

			//newV = (n + 1) * 100 / NSection;
			//uvs.Add(FVector2D(0, newV / 100));
			//uvs.Add(FVector2D(1, newV / 100));

			/* Debug */
			//UE_LOG(LogTemp, Warning, TEXT("0000 %f : %f"), n, newV/100);
			//UE_LOG(LogTemp, Warning, TEXT("2222 %f : %f"), n-1, newV1/100);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("v: %f"), FMath::Sin(90 / i + 1)));
			//UE_LOG(LogTemp, Warning, TEXT("uv3 U:%f, V:%f"), pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0.X, pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0.Y);

		}
		//else
		//{
		//	uvs.Add(FVector2D(0, FMath::Sin(0)));
		//	uvs.Add(FVector2D(1, FMath::Sin(0)));
		//}
	}
	UE_LOG(LogTemp, Warning, TEXT("load ver %d"), vertices.Num());
	UE_LOG(LogTemp, Warning, TEXT("load color %d"), vertexColors.Num());

	pm->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	FString m = msData.Material;
	Material = LoadObject<UMaterialInterface>(nullptr, *m);
	pm->SetMaterial(0, Material);
	pm->SetCollisionConvexMeshes({ vertices });
	bMerged = true;
	///////////////////////////////////////////////////////////////////////////////////////////
		//Center Position and Normal for Haptic 
	TotalVertice = msData.vertices;
	TotalNormal = msData.normals;
	centerPos = msData.centerPosition;
	centerNormals = msData.centerNormal;
	ArrMeshesections.color = msData.color;
	ArrMeshesections.Material = msData.Material;
	/*
		for (int i=0;i<centerPos.Num(); i++)
		{
			DrawDebugPoint(GetWorld(), centerPos[i], 5.5f, FColor::Black, true, 0, 0);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *centerPos[i].ToString());

		}*/

}