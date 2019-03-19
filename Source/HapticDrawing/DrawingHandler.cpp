// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "Engine/World.h"


// Sets default values
ADrawingHandler::ADrawingHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->brushinfo = new FBrushInfo(Draw, 50, FVector(255, 255, 255));
	prvPositon = FVector(0, 0, 0);
	isDrawing = false;
}

void ADrawingHandler::receivedFbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("receivedFbutton : I'm Drawing handler"));
	if (!hasClicked)
	{
		if (!(prvPositon == position))
		{
			generateStroke(position, rotation);
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("clicking!"));
		regenerateStroke(position, rotation);
	}
		
	prvPositon = position;

}

void ADrawingHandler::receivedSbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("receivedSbutton : I'm Drawing handler"));
}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("generate Stroke"));
	//AMyProcedualMesh* mesh1 = GetWorld()->SpawnActor<AMyProcedualMesh>(AMyProcedualMesh::StaticClass());
	isDrawing = true;
	AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	StrokeArray.Add(FStroke(position, position, mesh1));
	mesh1->Initialize(position, rotation);
	UE_LOG(LogTemp, Warning, TEXT("In array: %d"), StrokeArray.Num());

}

void ADrawingHandler::regenerateStroke(FVector position, FRotator rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));
	StrokeArray.Last().endPos = position;
	if (StrokeArray.Last().mesh->Destroy())
	{
		AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
		mesh1->Initialize(StrokeArray.Last().startPos, StrokeArray.Last().endPos, rotation);
		StrokeArray.Last().mesh = mesh1;
	}

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
