// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "UObject/Interface.h"
#include "ProceduralMeshComponent.h"
#include "ObjectAndNameAsStringProxyArchive.h"
#include "USaveableActorInterface.generated.h"

// Add default functionality here for any IUSaveableActorInterface functions that are not pure virtual.
UINTERFACE(BlueprintType)
class HAPTICDRAWING_API USaveableActorInterface : public UInterface
{
	GENERATED_BODY()
};

class HAPTICDRAWING_API ISaveableActorInterface


{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saveable Actor")
		void ActorSaveDataLoaded();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Saveable Actor")
		void ActorSaveDataSaved();
};

//Save and load

USTRUCT()
struct FMeshSectionData
{
	GENERATED_BODY()
	TArray<FVector> vertices;
	TArray<FVector> normals;
	FLinearColor color;
	TArray<FVector> centerPosition;
	TArray<FVector> centerNormal;
	FString Material;

	friend FArchive& operator<<(FArchive& Ar, FMeshSectionData& Data)
	{
		Ar << Data.vertices;
		Ar << Data.normals;
		Ar << Data.color;
		//Data.color.Serialize(Ar);
		Ar << Data.centerPosition;
		Ar << Data.centerNormal;
		Ar << Data.Material;

		return Ar;
	}
};


USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

	FString ActorClass;
	FName ActorName;
	FTransform ActorTransform;
	FMeshSectionData ProcMeshSections;
	//TArray<FVector> Vertices;
	//FString MeshMaterial;
	TArray<uint8> ActorData;

	friend FArchive& operator<<(FArchive& Ar, FActorSaveData& ActorData)
	{
		Ar << ActorData.ActorClass;
		Ar << ActorData.ActorName;
		Ar << ActorData.ActorTransform;
		Ar << ActorData.ProcMeshSections;
	/*	Ar << ActorData.Vertices;
		Ar << ActorData.MeshMaterial;*/
		Ar << ActorData.ActorData;
		return Ar;
	}
};

USTRUCT()
struct FSaveGameData
{
	GENERATED_BODY()

	FName GameID;
	FDateTime Timestamp;
	TArray<FActorSaveData> SavedActors;

	friend FArchive& operator<<(FArchive& Ar, FSaveGameData& GameData)
	{
		Ar << GameData.GameID;
		Ar << GameData.Timestamp;
		Ar << GameData.SavedActors;
		return Ar;
	}
};

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};

