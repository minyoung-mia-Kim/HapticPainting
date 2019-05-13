// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralPlaneMesh.h"
#include "DrawingHandler.generated.h"

UENUM()
enum BRUSHSTATE
{
	Draw,
	Recolor,
	Resize
};

USTRUCT()
struct FBrushInfo
{
	GENERATED_BODY()
		FBrushInfo() {}
	FBrushInfo(BRUSHSTATE _state, FString _mode, float _size, FLinearColor _color)
	{
		state = _state;
		mode = _mode;
		size = _size;
		color = _color;
	}
	BRUSHSTATE state;
	FString mode;
	float size;
	FLinearColor color;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBrushInfoDelegate, float, brushSize, FLinearColor, brushColor);

UCLASS()
class HAPTICDRAWING_API ADrawingHandler : public AActor
{
	GENERATED_BODY()

	//	Drawing stroke array
	UPROPERTY()
		TArray<FStroke> StrokeArray;
	UPROPERTY()
		TArray<FVector> PositionArray;
	UPROPERTY()
		TArray<FRotator> RotationArray;
	
	//	Brush list
	UPROPERTY()
		TArray<FString> BrushArray;
	//UPROPERTY()
	//	FStroke& CurrentStroke = nullptr;


public:
	// Sets default values for this actor's properties
	ADrawingHandler();

	UPROPERTY()
		FBrushInfoDelegate FBrushUpdateDelegate;

	/* Deltatime for haptic button */
	float dt = 0.0f;
	float prvDt = 0.0f;
	float FButtonDt = 0.0f;
	float SButtonDt = 0.0f;

	//Drawing Data
	FBrushInfo* brushinfo;
	bool isResizing;
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
	template<char key>
	void ChangeBrushMode();
	void ChangeBrushMode(char key);
	UFUNCTION()
		void BrushsizeUp(float val);
	UFUNCTION()
		void BrushsizeDown(float val);
	UFUNCTION()
		void UndoStroke();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void ChangeColorR();
	UFUNCTION()
		void ChangeColorG();
	UFUNCTION()
		void ChangeColorB();


};
