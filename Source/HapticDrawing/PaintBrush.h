// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

/* Cursor */
#include "Components/SphereComponent.h"

/* Brush */
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"


#include "PaintBrush.generated.h"

UCLASS()
class HAPTICDRAWING_API APaintBrush : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APaintBrush();

	/* Define Cursor and Brush*/
	UPROPERTY(EditDefaultsOnly, Category = "rc")
		USceneComponent* RComponent;

	UPROPERTY(VisibleAnywhere, Category = "Cursor")
		USphereComponent* cursor = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Cursor")
		UProceduralMeshComponent* brush = nullptr;

	FVector BrushNormal;
	UPROPERTY()
		float brushSize;
	UPROPERTY(EditAnywhere, Category = "MyProceduralMesh")
		UMaterialInterface* Material;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*
* Redraw the brush cursor
*/
	UFUNCTION()
		void RefreshBrushCursor(float bSize, FLinearColor brushColor, float viscosity, FString tex);

	UFUNCTION()
		void CreateBrushCursor(float bSize, FLinearColor brushColor, FString tex);
	

};
