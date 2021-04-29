// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "USaveableActorInterface.h"
#include "ConvertedStaticActor.h"
#include "ProceduralPlaneMesh.generated.h"


UCLASS()
class HAPTICDRAWING_API AProceduralPlaneMesh : public AActor, public ISaveableActorInterface
{
	GENERATED_BODY()


	UPROPERTY(SaveGame)
	int VariableToSave;

	UPROPERTY(VisibleAnywhere, Category = "MyProceduralMesh")
	UProceduralMeshComponent* pm = nullptr;
	UPROPERTY(EditAnywhere, Category = "MyProceduralMesh")
	UMaterialInterface* Material;
	
public:	
	// Sets default values for this actor's properties
	AProceduralPlaneMesh();
	UPROPERTY()
		TArray<FVector> vertices;
	UPROPERTY()
		TArray<FVector> normals;
	UPROPERTY()
		TArray<int32> triangles;
	UPROPERTY()
		TArray<FVector2D> uvs;
	UPROPERTY()
		TArray<FLinearColor> vertexColors;
	UPROPERTY()
		TArray<FProcMeshTangent> tangents;

	// Decide the resolution of Plane = the # of vertices
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		int32 height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		int32 width;
	int prvHeight;
	int prvWidth;
	int nGeneratedSection;
	// The distace btw vertices
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		float spacing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		bool generateMesh;

	/*Mesh's center location list*/
	UPROPERTY()
		TArray<FVector> centerPos;
	UPROPERTY()
		TArray<FVector> centerNormals;
	FVector prvPos;

	// To set expand section number
	bool bSetExpandNum;
	int startExpandNum;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FString mode);
	void Update(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FVector startPosition);
	void MergeSections();


	UPROPERTY()
	bool bMerged;

	virtual void OnConstruction(const FTransform& Transform) override;
	//void GenerateVertices();
	//void GenerateVertices(FVector position, FRotator rotation);

	void GenerateTriangles();
	void GenerateOppositeTriangles();
	void ClearMeshData();
	void SetHidden(bool bHidden);

	// Convert procedural mesh to static mesh
	UFUNCTION(BlueprintCallable, Category = "")
		void ConvertProceduralMeshToStaticMesh();
	
	FString AssetName = "";

public:
	//Save and Load
	virtual void ActorSaveDataLoaded_Implementation() override;
	virtual void ActorSaveDataSaved_Implementation() override;
	FMeshSectionData ArrMeshesections;

	TArray<FVector> TotalVertice;
	TArray<FVector> TotalNormal;

	FVector MeshColor;
	FMeshSectionData getAllMeshsections();

	TArray<int32> GetProcIndexbuffer();
	void LoadMeshsections(FMeshSectionData vertices);


public:
	// Optimization
	FString currentMaterial = "";
	void StoreDegeneratedSection();
	void DegenerateSection(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FVector startPosition);
	void UndoSection();

	// load data optimization
	void StoreLoadDataDegenSection();
	void MergeLoadSections();

	bool bMeshSectionCreated;
	bool bUndoSectionCalled;
	
	// degenerated section index
	TArray<int32> degenSectionIndex;
	// num of sections in stroke
	TArray<int32> degenSectionNum;
	TArray<FVector> startPositionArr;
	TArray<int> expandNumArr;
	FVector startPos;
	TArray<FLinearColor> TotalColors;
	TArray<FVector2D> FirstUVs;	

	int prevTotalNum = 0;
};
