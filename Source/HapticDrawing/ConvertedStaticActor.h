// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConvertedStaticActor.generated.h"

//DECLARE_DELEGATE_OneParam(FStaticInfoDelegate, FString);


UCLASS()
class HAPTICDRAWING_API AConvertedStaticActor : public AActor
{
	GENERATED_BODY()
	
public:	
	UStaticMeshComponent* sm;
	UStaticMesh* ConvertedStaticMesh;
	FString AssetName = "StaticMesh_From_ProceduralPlaneMesh_0";
protected:
	AConvertedStaticActor(const FObjectInitializer & ObjectInitializer);

//	void FindAndSetStaticMesh(const FObjectInitializer & ObjectInitializer);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void SetAssetNameAndStaticMesh(FString name, UStaticMesh* mesh);

	void SetStaticMesh();
};
