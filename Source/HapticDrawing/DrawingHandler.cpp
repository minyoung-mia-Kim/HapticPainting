// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "ProceduralPlaneMesh.h"
#include "Engine/World.h"


// Sets default values
ADrawingHandler::ADrawingHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->brushinfo = new BrushInfo(Draw, 50, FVector(255, 255, 255));

}

void ADrawingHandler::receivedFbutton(FVector position, FRotator rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("receivedFbutton : I'm Drawing handler"));
	generateStroke(position, rotation);
}

void ADrawingHandler::receivedSbutton(FVector position, FRotator rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("receivedSbutton : I'm Drawing handler"));
}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation)
{
	//AMyProcedualMesh* mesh1 = GetWorld()->SpawnActor<AMyProcedualMesh>(AMyProcedualMesh::StaticClass());
	AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	mesh1->Initialize(position, rotation);
}

void ADrawingHandler::EraseStroke()
{
}

// Called when the game starts or when spawned
void ADrawingHandler::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : I'm Drawing handler"));

}

// Called every frame
void ADrawingHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

