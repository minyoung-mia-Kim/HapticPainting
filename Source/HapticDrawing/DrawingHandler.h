// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralPlaneMesh.h"
#include "Archive.h"
#include "FileHelper.h"
#include "BufferArchive.h"
#include "USaveableActorInterface.h"
#include "TimerManager.h"
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
	FBrushInfo(BRUSHSTATE _state, int _mode, float _vis, float _size, FLinearColor _color)
	{
		state = _state;
		type = _mode;
		viscosity = _vis;
		size = _size;
		color = _color;
	}
	BRUSHSTATE state;
	int type;
	float viscosity;
	float size;
	FLinearColor color;

};

USTRUCT()
struct FStroke
{
	GENERATED_BODY()
		FStroke() {}
	FStroke(AProceduralPlaneMesh* _mesh)
	{
		mesh = _mesh;
	}

	AProceduralPlaneMesh* mesh;
};

UENUM()
enum BRUSHTYPE
{
	GRUNGE,
	BASIC,
	SILKY1,
	SILKY2,
	GLOW,
	FLOW
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBrushInfoDelegate, float, brushSize, FLinearColor, brushColor, float, viscosity, FString, brushTex);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHapticModeDelegate, FVector, brushPos);


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
	UPROPERTY()
		TArray<float> ViscosityArray;
	//UPROPERTY()
	//	FStroke& CurrentStroke = nullptr;

	//Haptic Mode
	bool isHapticMode;

public:
	// Sets default values for this actor's properties
	ADrawingHandler();

	UPROPERTY()
		FBrushInfoDelegate FBrushUpdateDelegate;

	////
	FVector startPosition;
	FVector endPosition;
	//// 

	/* Deltatime for haptic button */
	UPROPERTY()
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
	bool bFbuttonOff = true;
	UFUNCTION()
		void receivedFbutton(FVector position, FRotator rotation, bool hasClicked);
	UFUNCTION()
		void FbuttonOff();
	UFUNCTION()
		void receivedTriggerOn(FVector position, FRotator rotation, bool hasClicked);
	UFUNCTION()
		void receivedTriggerOff();

	UFUNCTION()
		void receivedSbutton(FVector position, FRotator rotation, bool hasClicked);
	UFUNCTION()
		void generateStroke(FVector position, FRotator rotation, FVector direction, FVector startPosition);
	UFUNCTION()
		void extendStroke(FVector position, FRotator rotation, FVector direction, FVector startPosition);

	UFUNCTION()
	void ChangeBrushMode(int tex);

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

	/* Color */
	UFUNCTION()
		void SetBrushColor(FLinearColor sColor, float sSize);

	/* Deprecated */
	UFUNCTION()
		void ChangeColorR();
	UFUNCTION()
		void ChangeColorG();
	UFUNCTION()
		void ChangeColorB();

public:
	//Save and Load
	TArray<FActorSaveData> SavedActors;
	void ActorSaveDataLoaded();
	void ActorSaveDataSaved();

	UFUNCTION()
		void Loaded(FSaveGameData SaveGameData);

	//Replay
	bool bReplay = false;
	int index = 0;
	float RprevDt = 0.0f;
	UFUNCTION()
		void ReplayPainting();
	UFUNCTION()
		void ShowPainting();
	//FTimerDelegate  TimerDel;
	FTimerHandle TimerHandle;

	UPROPERTY()
		FString AssetName;

	// effect material
	void EffectMaterialGlow();
	void EffectMaterialFlow();

	// mesh for each material
	TArray<AProceduralPlaneMesh*> meshes;
	TArray<int> previousBrushType;
	int currentBrushType;

	// save and load optimization
	void Mesh1();
	void Mesh2();
	void Mesh3();
	void Mesh4();

	// For Surface Reconstruction
	void SavePointCloud();
	void SaveToObj();

	FString ComputeInternalDivisionPoint(FVector p1, FVector p2, float ratio);
	FString ComputeInternalDivisionPointColor(FLinearColor c1, FLinearColor c2, float ratio);
};
