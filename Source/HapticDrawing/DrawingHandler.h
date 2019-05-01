// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralPlaneMesh.h"
#include "DrawingHandler.generated.h"

UENUM()
enum BRUSHMODE
{
	Draw,
	Eraser,
	Recolor
};

USTRUCT()
struct FBrushInfo
{
	GENERATED_BODY()
		FBrushInfo() {}
	FBrushInfo(BRUSHMODE _mode, float _size, FVector _color)
	{
		mode = _mode;
		size = _size;
		color = _color;
	}
	BRUSHMODE mode;
	float size;
	FVector color;
};

USTRUCT()
struct FStroke
{
	GENERATED_BODY()
		FStroke() {}
	FStroke(FVector sPos, FVector ePos, AProceduralPlaneMesh* _mesh)
	{
		mesh = _mesh;
		startPos = sPos;
		endPos = ePos;
	}

	AProceduralPlaneMesh* mesh;
	FVector startPos;
	FVector endPos;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBrushDelegate, float, brushSize);


UCLASS()
class HAPTICDRAWING_API ADrawingHandler : public AActor
{
	GENERATED_BODY()


	UPROPERTY()
		TArray<FStroke> StrokeArray;
	UPROPERTY()
		TArray<FVector> PositionArray;
	UPROPERTY()
		TArray<FRotator> RotationArray;
	//UPROPERTY()
	//	FStroke& CurrentStroke = nullptr;


public:
	// Sets default values for this actor's properties
	ADrawingHandler();

	UPROPERTY()
		FBrushDelegate FBrushSizeDelegate;

	float dt = 0.0f;
	float prvDt = 0.0f;
	//Drawing Data
	FBrushInfo* brushinfo;
	//Drawing function
	FVector prvPositon;
	FVector DrawingDirection;
	UFUNCTION()
		void receivedFbutton(FVector position, FRotator rotation, bool hasClicked);
	UFUNCTION()
		void receivedSbutton(FVector position, FRotator rotation, bool hasClicked);
	UFUNCTION()
		void generateStroke(FVector position, FRotator rotation, FVector direction);
	UFUNCTION()
		void regenerateStroke(FVector position, FRotator rotation, FVector direction);
	UFUNCTION()
		void EraseStroke();
	UFUNCTION()
		void BrushsizeUp();
	UFUNCTION()
		void BrushsizeDown();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		// Called every frame
	virtual void Tick(float DeltaTime) override;



};
