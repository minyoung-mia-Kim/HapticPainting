// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DrawingHandler.generated.h"

UCLASS()
class HAPTICDRAWING_API ADrawingHandler : public AActor
{
	GENERATED_BODY()
	enum BRUSHMODE
	{
		Draw,
		Eraser,
		Recolor
	};
	struct BrushInfo
	{
		BrushInfo(BRUSHMODE _mode, float _size, FVector _color)
		{
			mode = _mode;
			size = _size;
			color = _color;
		}
		BRUSHMODE mode;
		float size;
		FVector color;
	};
	struct Stroke
	{
		Stroke();
		float width;
		float height;
		float spacing = 10.f;
	};
public:	
	// Sets default values for this actor's properties
	ADrawingHandler();

	//Drawing Data
	BrushInfo* brushinfo;
	//Drawing function
	void generateStroke(FVector position);
	void EraseStroke();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
