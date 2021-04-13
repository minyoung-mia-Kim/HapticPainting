// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralPlaneMesh.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

// to convert procedural mesh component to static mesh component
#include "RawMesh.h"
#include "AssetRegistryModule.h"
#include "Classes/Engine/StaticMesh.h"

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
	
	bMerged = false;

	startExpandNum = 0;
	bSetExpandNum = false;
	bMeshSectionCreated = false;
	bUndoSectionCalled = false;
	
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

// Initialize the mesh's location
void AProceduralPlaneMesh::Initialize(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FString mode)
{
	ClearMeshData();
	float uvSpacing = 1.0f / FMath::Max(height, width);

	//	Set material
	if (currentMaterial == "")
	{
		currentMaterial = mode;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *mode);
	}
	
	Material = LoadObject<UMaterialInterface>(nullptr, *mode);
	pm->SetMaterial(0, Material);

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)))));
	//vertexColors.Add(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); //red

	vertices.Add(FVector(position + GetTransform().TransformVector(rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)))));
	//vertexColors.Add(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); //green
	uvs.Add(FVector2D(0, 0));
	uvs.Add(FVector2D(1, 0));
	prvPos = position;

	TotalColors.Add(color);
	TotalColors.Add(color);

	//UE_LOG(LogTemp, Warning, TEXT("??????"));

	/*Save mesh section data*/
	TotalVertice.Append(vertices);

	/*material saveeee*/
	ArrMeshesections.color = color;
	ArrMeshesections.Material = mode;
}

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


// store degenerated section index and number of sections in each stroke
void AProceduralPlaneMesh::StoreDegeneratedSection()
{
	if (degenSectionIndex.Num() == 0)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Called %d"), nGeneratedSection);
		degenSectionIndex.Add(nGeneratedSection + 1);
	}
	else
	{
		degenSectionIndex.Add(degenSectionIndex.Last() + nGeneratedSection);
	}
	
	degenSectionNum.Add(nGeneratedSection);
}

// degenerate section when drawing
void AProceduralPlaneMesh::DegenerateSection(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FVector startPosition)
{
	nGeneratedSection = 0;
	bSetExpandNum = false;

	startPos = startPosition;

	float uvSpacing = 1.0f / FMath::Max(nGeneratedSection, width);

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));  // Question!
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2))); // Question!

	/* Normal and Tangent */
	//Normal 1 : Mesh front - Forward
	FVector Normal = FVector::CrossProduct(FVector(vertices[1] - vertices[3]), FVector(vertices[2] - vertices[3])); //31, 32
	Normal.Normalize();

	//Normal 2 : Hapatic - Forward
	FVector HapticOrientation = -rotation.Vector();

	if (FVector::DotProduct(Normal, HapticOrientation) < 0.0)
		Normal = -Normal;

	FVector surfaceTangent = vertices[2] - vertices[3]; //p1 to p3 being FVectors
	//	Gets a normalized copy of the vector, checking it is safe to do so based on the length. 
	//	Returns zero vector if vector length is too small to safely normalize.
	surfaceTangent = surfaceTangent.GetSafeNormal();

	if (surfaceTangent == FVector().ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("vector length is too small to safely normalize"));

	// add normal, color, and tangent value for 4 vertices of the section
	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			normals.Add(Normal);
			vertexColors.Add(color);
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
		}
	}
	//	Two-sided front-faces
	GenerateTriangles();
	GenerateOppositeTriangles();

	TotalColors.Add(color);
	TotalColors.Add(color);
	
	int temp = nGeneratedSection + 1;
	
	/* Add a mesh section */
	pm->CreateMeshSection_LinearColor(temp, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	
	//nGeneratedSection = 2;

	pm->SetCollisionConvexMeshes({ vertices }); // Question

	/* Store mesh's location */
	centerPos.Add(FVector((prvPos + position) / 2));
	centerNormals.Add(Normal);
	prvPos = position;

	/* increase section idx */
	nGeneratedSection++;

	/* Clean mesh data for next mesh section*/
	ClearMeshData();

	/* bottom vertice data for the next mesh section */
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));
	
	/*Save mesh section data*/
	TotalVertice.Append(vertices);
}

void AProceduralPlaneMesh::Update(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FVector startPosition)
{
	/* Mesh component's coordination is always 0, 0, 0 since its not moving */
	float uvSpacing = 1.0f / FMath::Max(nGeneratedSection, width);

	// The distance between this mesh component and the haptic position
	// Actually, it's the same with the haptic position itself since the mesh component coordination is (0,0,0).
	//FVector dis = GetTransform().TransformVector(position);

	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, spacing / 2)));
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));


	/* Normal and Tangent */
	// vertice[0] & vertice[1] = previous position, vertices[2] & vertices[3] = current position
	//Normal 1 : Mesh front - Forward
	FVector Normal = FVector::CrossProduct(FVector(vertices[1] - vertices[3]), FVector(vertices[2] - vertices[3])); //31, 32
	Normal.Normalize();

	//Normal 2 : Hapatic - Forward
	FVector HapticOrientation = -rotation.Vector();

	if (FVector::DotProduct(Normal, HapticOrientation) < 0.0)
		Normal = -Normal;


	FVector surfaceTangent = vertices[2] - vertices[3]; //p1 to p3 being FVectors

	//	Gets a normalized copy of the vector, checking it is safe to do so based on the length. 
	//	Returns zero vector if vector length is too small to safely normalize.
	surfaceTangent = surfaceTangent.GetSafeNormal();

	if (surfaceTangent == FVector().ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("vector length is too small to safely normalize"));

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

	// to save color data
	TotalColors.Add(color);
	TotalColors.Add(color);

	/* Add a mesh section */
	pm->CreateMeshSection_LinearColor(nGeneratedSection, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	pm->SetCollisionConvexMeshes({ vertices });

	// UVs
	float NSection = float(pm->GetNumSections());
	float newV;
	float newV1;

	startPos = startPosition;

	if (startPositionArr.Num() == 0 || startPositionArr.Last() != startPos)
		startPositionArr.Add(startPos);

	if (FVector::Dist(startPos, position) > 10.0f && !bSetExpandNum) {
		bSetExpandNum = true;
		startExpandNum = NSection;
		expandNumArr.Add(startExpandNum);
	}

	int32 temp = 0;
	if (degenSectionIndex.Num() != 0)
	{
		temp = 1;
	}
	
	for (int i = pm->GetNumSections() - 1; i > temp; i--)
	{
		if (i != 0)
		{
			float n = float(i + 1);
			
			if (!bSetExpandNum || (NSection < 2 * expandNumArr.Last()))
			{
				newV1 = (n - 1) * 100 / NSection;
				newV = n * 100 / NSection;
				if (pm->GetProcMeshSection(i)->ProcVertexBuffer.Num() != 0)
				{
					pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, newV1 / 100);
					pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, newV1 / 100);
					pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, newV / 100);
					pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, newV / 100);
				}
			}
			else
			{
				if (n < expandNumArr.Last())
				{
					newV1 = (n - 1) * 100 / expandNumArr.Last();
					newV = n * 100 / expandNumArr.Last();
					if (pm->GetProcMeshSection(i)->ProcVertexBuffer.Num() != 0)
					{
						pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, 0.49 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, 0.49 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, 0.49 * newV / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, 0.49 * newV / 100);
					}
				}
				else if (n > NSection - expandNumArr.Last())
				{
					newV1 = (n - 1 - NSection + expandNumArr.Last()) * 100 / expandNumArr.Last();
					newV = (n - NSection + expandNumArr.Last()) * 100 / expandNumArr.Last();
					if (pm->GetProcMeshSection(i)->ProcVertexBuffer.Num() != 0)
					{
						pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, 0.51 + 0.49 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, 0.51 + 0.49 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, 0.51 + 0.49 * newV / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, 0.51 + 0.49 * newV / 100);
					}
				}
				else
				{
					newV1 = (n - 1 - expandNumArr.Last()) * 100 / (NSection - expandNumArr.Last());
					newV = (n - expandNumArr.Last()) * 100 / (NSection - expandNumArr.Last());
					if (pm->GetProcMeshSection(i)->ProcVertexBuffer.Num() != 0)
					{
						pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, 0.49 + 0.02 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, 0.49 + 0.02 * newV1 / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[2].UV0 = FVector2D(0, 0.49 + 0.02 * newV / 100);
						pm->GetProcMeshSection(i)->ProcVertexBuffer[3].UV0 = FVector2D(1, 0.49 + 0.02 * newV / 100);
					}
				}
			}
		}
		else
		{
			pm->GetProcMeshSection(i)->ProcVertexBuffer[0].UV0 = FVector2D(0, FMath::Sin(0));
			pm->GetProcMeshSection(i)->ProcVertexBuffer[1].UV0 = FVector2D(1, FMath::Sin(0));
		}
	}

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
	vertices.Add(FVector(position.X, position.Y, position.Z) + rotation.RotateVector(FVector(0.0f, 0.0f, -spacing / 2)));

	/*Save mesh section data*/
	TotalVertice.Append(vertices);
}

void AProceduralPlaneMesh::MergeSections()
{
	if (bMerged || TotalNormal.Num() == 0)
	{
		return;
	}
		
	int sectionNum = 0;
	ClearMeshData();
	pm->ClearAllMeshSections();

	if (!bSetExpandNum)
	{
		bSetExpandNum = true;
		expandNumArr.Add(0);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Brush UP"));
	// when expand section num is not stored
	if (degenSectionNum.Num() != 0 && degenSectionNum.Num() != expandNumArr.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("not stored"));
		int curTotalNum = TotalVertice.Num();
		
		for (int i = curTotalNum; i > prevTotalNum; i--)
		{
			TotalVertice.RemoveAt(TotalVertice.Num() - 1);
			TotalNormal.RemoveAt(TotalNormal.Num() - 1);
			TotalColors.RemoveAt(TotalColors.Num() - 1);
		}

		degenSectionIndex.RemoveAt(degenSectionIndex.Num() - 1);
		degenSectionNum.RemoveAt(degenSectionNum.Num() - 1);

		if (TotalNormal.Num() >= 2) {
			TotalNormal.RemoveAt(TotalNormal.Num() - 1);
			TotalNormal.RemoveAt(TotalNormal.Num() - 1);
		}
		
		if (TotalVertice.Num() >= 2) {
			vertices.Add(TotalVertice[TotalVertice.Num() - 2]);
			vertices.Add(TotalVertice.Last());
		}
		

		MergeSections();
		return;
	}

	FVector Normal;
	FVector surfaceTangent;
	for (int i = 0; i < TotalVertice.Num(); i += 2)
	{

		bool bDegenerateSection = false;
		for (int k = 0; k < degenSectionIndex.Num(); k++)
		{
			if (i == (degenSectionIndex[k] * 2 - 2))
				bDegenerateSection = true;
		}

		if (bDegenerateSection)
		{
			i += 2;
		}
		else if (i + 2 < TotalVertice.Num())
		{
			sectionNum++;

			surfaceTangent = TotalVertice[i + 2] - TotalVertice[i + 3]; //p1 to p3 being FVectors
			surfaceTangent = surfaceTangent.GetSafeNormal();

			for (int32 y = 0; y < height; y++)
			{
				vertexColors.Add(ArrMeshesections.color);
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
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
		}
	}

	//UVS
	float NSection = float(sectionNum);
	float newV1;

	if (degenSectionNum.Num() == 0 && expandNumArr.Num() == 1)
	{
		for (int i = 0; i < TotalVertice.Num() / 2; i++)
		{

			float n = float(i);
			if (expandNumArr[0] == 0)
			{
				newV1 = (n - 1) * 100 / NSection;
				newV1 = n * 100 / NSection;
				uvs.Add(FVector2D(0, newV1 / 100));
				uvs.Add(FVector2D(1, newV1 / 100));
			}
			else {
				if (n < expandNumArr[0])
				{
					newV1 = n * 100 / expandNumArr[0];
					uvs.Add(FVector2D(0, 0.49 * newV1 / 100));
					uvs.Add(FVector2D(1, 0.49 * newV1 / 100));
				}
				else if (n > NSection - expandNumArr[0])
				{
					newV1 = (n - NSection + expandNumArr[0]) * 100 / expandNumArr[0];
					uvs.Add(FVector2D(0, 0.51 + 0.49 * newV1 / 100));
					uvs.Add(FVector2D(1, 0.51 + 0.49 * newV1 / 100));
				}
				else
				{
					newV1 = (n - expandNumArr[0]) * 100 / (NSection - expandNumArr[0]);
					uvs.Add(FVector2D(0, 0.49 + 0.02 * newV1 / 100));
					uvs.Add(FVector2D(1, 0.49 + 0.02 * newV1 / 100));
				}
			}
		}
		FirstUVs.Append(uvs);
	}
	else
	{
		uvs.Append(FirstUVs);

		uvs.Add(FVector2D(0, 0));
		uvs.Add(FVector2D(1, 0));
				
		for (int i = 0; i < degenSectionNum.Num(); i++)
		{
			for (int k = 1; k <= degenSectionNum[i]; k++)
			{
				float n = float(k);
				if (expandNumArr[i] == 0)
				{
					newV1 = n * 100 / degenSectionNum[i];
					uvs.Add(FVector2D(0, newV1 / 100));
					uvs.Add(FVector2D(1, newV1 / 100));
				}
				else
				{
					if (n < expandNumArr[i])
					{
						newV1 = n * 100 / expandNumArr[i];
						uvs.Add(FVector2D(0, 0.49 * newV1 / 100));
						uvs.Add(FVector2D(1, 0.49 * newV1 / 100));
					}
					else if (n > degenSectionNum[i] - expandNumArr[i])
					{
						newV1 = (n - degenSectionNum[i] + expandNumArr[i]) * 100 / expandNumArr[i];
						uvs.Add(FVector2D(0, 0.51 + 0.49 * newV1 / 100));
						uvs.Add(FVector2D(1, 0.51 + 0.49 * newV1 / 100));
					}
					else
					{
						newV1 = (n - expandNumArr[i]) * 100 / (degenSectionNum[i] - expandNumArr[i]);
						uvs.Add(FVector2D(0, 0.49 + 0.02 * newV1 / 100));
						uvs.Add(FVector2D(1, 0.49 + 0.02 * newV1 / 100));
					}
				}
			}
		}
	}

	if (!bUndoSectionCalled)
	{
		TotalNormal.Add(FVector(TotalNormal.Last()));
		TotalNormal.Add(FVector(TotalNormal.Last()));
	}

	//Check whether each feature has the same number of data with its vertices
	UE_LOG(LogTemp, Warning, TEXT("TotalVertice %d"), TotalVertice.Num());
	UE_LOG(LogTemp, Warning, TEXT("TotalNormal %d"), TotalNormal.Num());
	UE_LOG(LogTemp, Warning, TEXT("TotalColors : %d"), TotalColors.Num());
	//UE_LOG(LogTemp, Warning, TEXT("uvs %d"), uvs.Num());
	//UE_LOG(LogTemp, Warning, TEXT("vertexColors %d"), vertexColors.Num());
	//UE_LOG(LogTemp, Warning, TEXT("tangents %d"), tangents.Num());

	//TotalNormal.RemoveAt(TotalNormal.Num() - 1);
	//TotalNormal.RemoveAt(TotalNormal.Num() - 1);

	pm->CreateMeshSection_LinearColor(0, TotalVertice, triangles, TotalNormal, uvs, TotalColors, tangents, true);

	FString m = ArrMeshesections.Material;
	Material = LoadObject<UMaterialInterface>(nullptr, *m);

	pm->SetMaterial(0, Material);
	pm->SetCollisionConvexMeshes({ vertices });

	vertices.Add(TotalVertice[TotalVertice.Num() - 2]);
	vertices.Add(TotalVertice.Last());

	prevTotalNum = TotalVertice.Num();
	bMerged = true;
}

void AProceduralPlaneMesh::UndoSection()
{
	bUndoSectionCalled = true;

	int removeNum = 2 * degenSectionNum.Last();
	if (degenSectionIndex.Num() == 1)
	{
		removeNum += 2;
	}

	for (int i = 0; i < removeNum; i++)
	{
		TotalVertice.RemoveAt(TotalVertice.Num() - 1);
		TotalNormal.RemoveAt(TotalNormal.Num() - 1);
		TotalColors.RemoveAt(TotalColors.Num() - 1);
	}

	degenSectionIndex.RemoveAt(degenSectionIndex.Num() - 1);
	degenSectionNum.RemoveAt(degenSectionNum.Num() - 1);
	startPositionArr.RemoveAt(startPositionArr.Num() - 1);
	expandNumArr.RemoveAt(expandNumArr.Num() - 1);

	if (TotalVertice.Num() != 0)
	{
		MergeSections();
		bMerged = false;
	}

	bUndoSectionCalled = false;
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

// convert procedural mesh component to static mesh component
void AProceduralPlaneMesh::ConvertProceduralMeshToStaticMesh()
{
	UProceduralMeshComponent* ProcMeshComp = pm;
	FVector SpawnLoc = pm->GetComponentLocation();
	FRotator SpawnRot = pm->GetComponentRotation();
	FQuat SpawnQuat= pm->GetComponentQuat();
	if (ProcMeshComp != nullptr)
	{
		FString ActorName = pm->GetOwner()->GetName();
		FString LevelName = pm->GetWorld()->GetMapName();
		AssetName = FString(TEXT("StaticMesh_From")) + FString(TEXT("_") + ActorName);
		FString PathName = FString(TEXT("/Game/ConvertedStaticMesh/"));
		FString PackageName = PathName + AssetName;

		FRawMesh RawMesh;
		TArray<UMaterialInterface*> MeshMaterials;

		const int32 NumSections = ProcMeshComp->GetNumSections();
		int32 VertexBase = 0;
		for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
		{
			FProcMeshSection* ProcSection = ProcMeshComp->GetProcMeshSection(SectionIdx);

			for (FProcMeshVertex& Vert : ProcSection->ProcVertexBuffer)
			{
				RawMesh.VertexPositions.Add(Vert.Position);
			}

			int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
			for (int32 IndexIdx = 0; IndexIdx < NumIndices; IndexIdx++)
			{
				int32 Index = ProcSection->ProcIndexBuffer[IndexIdx];

				RawMesh.WedgeIndices.Add(Index + VertexBase);

				FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[Index];

				FVector TangentX = ProcVertex.Tangent.TangentX;
				FVector TangentZ = ProcVertex.Normal;
				FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal() * (ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f);

				RawMesh.WedgeTangentX.Add(TangentX);
				RawMesh.WedgeTangentY.Add(TangentY);
				RawMesh.WedgeTangentZ.Add(TangentZ);

				RawMesh.WedgeTexCoords[0].Add(ProcVertex.UV0);
				RawMesh.WedgeColors.Add(ProcVertex.Color);
			}

			int32 NumTris = NumIndices / 3;
			for (int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
			{
				RawMesh.FaceMaterialIndices.Add(SectionIdx);
				RawMesh.FaceSmoothingMasks.Add(0);
			}

			MeshMaterials.Add(ProcMeshComp->GetMaterial(SectionIdx));

			VertexBase += ProcSection->ProcVertexBuffer.Num();

			if (RawMesh.VertexPositions.Num() > 3 && RawMesh.WedgeIndices.Num() > 3)
			{
				UPackage* Package = CreatePackage(NULL, *PackageName);
				check(Package);

				UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, FName(*AssetName), RF_Public | RF_Standalone);
				StaticMesh->InitResources();

				StaticMesh->LightingGuid = FGuid::NewGuid();

				FStaticMeshSourceModel* SrcModel = new (StaticMesh->SourceModels) FStaticMeshSourceModel();
				SrcModel->BuildSettings.bRecomputeNormals = false;
				SrcModel->BuildSettings.bRecomputeTangents = false;
				SrcModel->BuildSettings.bRemoveDegenerates = false;
				SrcModel->BuildSettings.bUseHighPrecisionTangentBasis = false;
				SrcModel->BuildSettings.bUseFullPrecisionUVs = false;
				SrcModel->BuildSettings.bGenerateLightmapUVs = true;
				SrcModel->BuildSettings.SrcLightmapIndex = 0;
				SrcModel->BuildSettings.DstLightmapIndex = 1;
				SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);

				for (UMaterialInterface* Material : MeshMaterials)
				{
					StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
				}

				StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

				StaticMesh->Build(false);
				StaticMesh->PostEditChange();

				StaticMesh->bAutoComputeLODScreenSize = true;
				StaticMesh->LODForCollision = false;
				StaticMesh->SetLODGroup("SmallProp", true);

				FAssetRegistryModule::AssetCreated(StaticMesh);
					
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				
				// spawn converted static mesh actor
				AConvertedStaticActor* convertedStaticActor = GetWorld()->SpawnActor<AConvertedStaticActor>(AConvertedStaticActor::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
				convertedStaticActor->SetAssetNameAndStaticMesh(AssetName, StaticMesh);
				convertedStaticActor->bCollideWhenPlacing = false;

				convertedStaticActor->bEnableAutoLODGeneration = true;
			}
		}
	}

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

			surfaceTangent = vertices[i + 2] - vertices[i + 3]; //p1 to p3 being FVectors
			surfaceTangent = surfaceTangent.GetSafeNormal();


			for (int32 x = 0; x < width; x++)
			{
				vertexColors.Add(msData.color);
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
	float newV1;

	for (int i = 0; i < msData.vertices.Num() / 2; i++)
	{
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
	//UE_LOG(LogTemp, Warning, TEXT("load ver %d"), vertices.Num());
	//UE_LOG(LogTemp, Warning, TEXT("load color %d"), vertexColors.Num());

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
	TotalColors = vertexColors;
}

void AProceduralPlaneMesh::StoreLoadDataDegenSection()
{
	if (degenSectionNum.Num() == 0)
	{
		degenSectionNum.Add(TotalVertice.Num() / 2);
		degenSectionIndex.Add(TotalVertice.Num() / 2 + 1);
	}
	else
	{
		degenSectionNum.Add(TotalVertice.Num() / 2 - degenSectionIndex.Last());
		degenSectionIndex.Add(TotalVertice.Num() / 2 + 1);
	}

	UE_LOG(LogTemp, Warning, TEXT("degenSectionIndex[%d]: %d"), degenSectionIndex.Num() - 1, degenSectionIndex.Last());
	UE_LOG(LogTemp, Warning, TEXT("degenSectionNum[%d]: %d"), degenSectionNum.Num() - 1, degenSectionNum.Last());
}

// to merge loaded data sections
void AProceduralPlaneMesh::MergeLoadSections()
{
	int sectionNum = 0;
	ClearMeshData();
	pm->ClearAllMeshSections();

	FVector Normal;
	FVector surfaceTangent;

	for (int i = 0; i < TotalVertice.Num(); i += 2)
	{
		bool bDegenerateSection = false;
		for (int k = 0; k < degenSectionIndex.Num(); k++)
		{
			if (i == (degenSectionIndex[k] * 2 - 2))
				bDegenerateSection = true;
		}

		if (bDegenerateSection)
		{
			i += 2;
		}
		else if (i + 2 < TotalVertice.Num())
		{
			sectionNum++;

			surfaceTangent = TotalVertice[i + 2] - TotalVertice[i + 3]; //p1 to p3 being FVectors
			surfaceTangent = surfaceTangent.GetSafeNormal();

			for (int32 y = 0; y < height; y++)
			{
				vertexColors.Add(ArrMeshesections.color);
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
			tangents.Add(FProcMeshTangent(surfaceTangent, true));
			tangents.Add(FProcMeshTangent(surfaceTangent, true));

		}
	}

	//UVS
	float NSection = float(sectionNum);
	float newV1;

	if (degenSectionNum.Num() == 0)
	{
		for (int i = 0; i < TotalVertice.Num() / 2; i++)
		{

			float n = float(i);
			newV1 = n * 100 / NSection;
			uvs.Add(FVector2D(0, newV1 / 100));
			uvs.Add(FVector2D(1, newV1 / 100));
		}
		FirstUVs.Append(uvs);
	}
	else
	{
		uvs.Append(FirstUVs);


		for (int i = 0; i < degenSectionNum.Num(); i++)
		{
			for (int k = 1; k <= degenSectionNum[i]; k++)
			{
				float n = float(k);
				newV1 = n * 100 / degenSectionNum[i];
				uvs.Add(FVector2D(0, newV1 / 100));
				uvs.Add(FVector2D(1, newV1 / 100));
				if (i == 0)
					UE_LOG(LogTemp, Warning, TEXT("k=%d"), k);
			}
		}
		uvs.Add(FVector2D(0, 0));
		uvs.Add(FVector2D(1, 0));
	}

	if (TotalNormal.Num() != TotalVertice.Num())
	{
		TotalNormal.Add(FVector(TotalNormal.Last()));
		TotalNormal.Add(FVector(TotalNormal.Last()));
	}

	UE_LOG(LogTemp, Warning, TEXT("TotalVertice %d"), TotalVertice.Num());
	UE_LOG(LogTemp, Warning, TEXT("triangles: %f"), triangles.Num() / 3);
	UE_LOG(LogTemp, Warning, TEXT("TotalNormal %d"), TotalNormal.Num());
	UE_LOG(LogTemp, Warning, TEXT("uvs %d"), uvs.Num());
	UE_LOG(LogTemp, Warning, TEXT("TotalColors %d"), TotalColors.Num());
	UE_LOG(LogTemp, Warning, TEXT("tangents: %d"), tangents.Num());

	pm->CreateMeshSection_LinearColor(0, TotalVertice, triangles, TotalNormal, uvs, TotalColors, tangents, true);
	FString m = ArrMeshesections.Material;
	Material = LoadObject<UMaterialInterface>(nullptr, *m);
	pm->SetMaterial(0, Material);
	pm->SetCollisionConvexMeshes({ vertices });
}