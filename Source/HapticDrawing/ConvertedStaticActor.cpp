// Fill out your copyright notice in the Description page of Project Settings.

#include "ConvertedStaticActor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Classes/Engine/StaticMesh.h"

// Sets default values
AConvertedStaticActor::AConvertedStaticActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString pathName = "StaticMesh'/Game/Test/ClearMesh.ClearMesh'";
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ClearMesh(*pathName);

	sm = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("staticMeshes"));
	sm->SetStaticMesh(ClearMesh.Object);
	//sm->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AConvertedStaticActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AConvertedStaticActor::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void AConvertedStaticActor::SetAssetNameAndStaticMesh(FString name, UStaticMesh* mesh)
{
	AssetName = name;
	ConvertedStaticMesh = mesh;
	SetStaticMesh();
}

void AConvertedStaticActor::SetStaticMesh()
{
	FString pathName = "StaticMesh'/Game/ConvertedStaticMesh/" + AssetName + "." + AssetName + "'";
	sm->SetStaticMesh(ConvertedStaticMesh);
}