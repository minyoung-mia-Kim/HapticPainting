// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"


// Sets default values
ADrawingHandler::ADrawingHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->brushinfo = new BrushInfo(Draw, 50, FVector(255, 255, 255));
}

void ADrawingHandler::generateStroke(FVector position)
{
}

void ADrawingHandler::EraseStroke()
{
}

// Called when the game starts or when spawned
void ADrawingHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADrawingHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

