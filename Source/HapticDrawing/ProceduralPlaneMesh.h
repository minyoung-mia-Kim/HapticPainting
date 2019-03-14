// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralPlaneMesh.generated.h"

UCLASS()
class HAPTICDRAWING_API AProceduralPlaneMesh : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "MyProceduralMesh")
	UProceduralMeshComponent* pm;
	
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
	// The distace btw vertices
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		float spacing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyProceduralMesh")
		bool generateMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initialize(FVector position, FRotator rotation);
	virtual void OnConstruction(const FTransform& Transform) override;
	void GenerateVertices();
	void GenerateTriangles();
	void ClearMeshData();
	
};
