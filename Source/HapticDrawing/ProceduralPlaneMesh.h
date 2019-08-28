// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "USaveableActorInterface.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color, FString mode);
	void Update(FVector position, FRotator rotation, FVector direction, float spacing, FLinearColor color);
	void MergeSections();

	virtual void OnConstruction(const FTransform& Transform) override;
	//void GenerateVertices();
	//void GenerateVertices(FVector position, FRotator rotation);

	void GenerateTriangles();
	void GenerateOppositeTriangles();
	void ClearMeshData();
	void SetHidden(bool bHidden);
public:
	//Save and Load
	virtual void ActorSaveDataLoaded_Implementation() override;
	virtual void ActorSaveDataSaved_Implementation() override;
	FMeshSectionData ArrMeshesections;
	TArray<FVector> TotalVertice;
	FVector MeshColor;
	FMeshSectionData getAllMeshsections();

	void LoadMeshsections(FMeshSectionData vertices);

};
