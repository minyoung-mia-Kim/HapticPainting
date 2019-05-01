// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "Engine/World.h"


// Sets default values
ADrawingHandler::ADrawingHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->brushinfo = new FBrushInfo(Draw, 10.f, FVector(255, 255, 255));
	prvPositon = FVector(0, 0, 0);
}

void ADrawingHandler::receivedFbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//FVector Normal = -(position - FVector(40.f, 0.f, 0.f));
	DrawingDirection = FVector(position - prvPositon);

	if (!hasClicked)
	{
		if (!(DrawingDirection.Size() < 1.0))
		{
			generateStroke(position, rotation, DrawingDirection);
		}
	}
	else
	{
		if (dt - prvDt > 0.05 && StrokeArray.Num() > 0)
		{
			PositionArray.Add(position);
			RotationArray.Add(rotation);
			UE_LOG(LogTemp, Warning, TEXT("Direction X:%f, Y:%f, Z:%f"), DrawingDirection.X, DrawingDirection.Y, DrawingDirection.Z);

			if (FMath::Abs(FVector::Dist(position, prvPositon)))
				regenerateStroke(position, rotation, DrawingDirection);

			prvDt = dt;
			//UE_LOG(LogTemp, Warning, TEXT("clicking!"));
		}

	}

	prvPositon = position;

}

void ADrawingHandler::receivedSbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("receivedSbutton : I'm Drawing handler"));
	/*for (int i = 0; i < PositionArray.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("X:%f, Y:%f, Z:%f"), PositionArray[i].X, PositionArray[i].Y, PositionArray[i].Z);
	}
	UE_LOG(LogTemp, Warning, TEXT("Number of : %d"), PositionArray.Num());*/


	//AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	//mesh1->Initialize(PositionArray, RotationArray);
	BrushsizeUp();

}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation, FVector direction)
{
	UE_LOG(LogTemp, Warning, TEXT("generate Stroke"));
	//AMyProcedualMesh* mesh1 = GetWorld()->SpawnActor<AMyProcedualMesh>(AMyProcedualMesh::StaticClass());
	AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	StrokeArray.Add(FStroke(position, position, mesh1));
	mesh1->Initialize(position, rotation, direction, brushinfo->size);
	UE_LOG(LogTemp, Warning, TEXT("In array: %d"), StrokeArray.Num());

}

void ADrawingHandler::regenerateStroke(FVector position, FRotator rotation, FVector direction)
{
	UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));
	StrokeArray.Last().mesh->Update(position, rotation, direction, brushinfo->size);
}

void ADrawingHandler::EraseStroke()
{
}

void ADrawingHandler::BrushsizeUp()
{
	brushinfo->size += 0.005f;
	UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
	FBrushSizeDelegate.Broadcast(brushinfo->size);
}

void ADrawingHandler::BrushsizeDown()
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
	dt += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%f"), dt);
}
