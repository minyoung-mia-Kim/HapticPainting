// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"


// Sets default values
ADrawingHandler::ADrawingHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->brushinfo = new FBrushInfo(Draw, 10.f, FLinearColor::White);
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
		if (dt - prvDt > 0.03 && StrokeArray.Num() > 0)
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

	if (!hasClicked)
	{
		BrushsizeUp();
	}
}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation, FVector direction)
{
	UE_LOG(LogTemp, Warning, TEXT("generate Stroke"));
	//AMyProcedualMesh* mesh1 = GetWorld()->SpawnActor<AMyProcedualMesh>(AMyProcedualMesh::StaticClass());
	AProceduralPlaneMesh* mesh1 = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	StrokeArray.Add(FStroke(position, position, mesh1));
	mesh1->Initialize(position, rotation, direction, brushinfo->size, brushinfo->color);
	UE_LOG(LogTemp, Warning, TEXT("In array: %d"), StrokeArray.Num());

}

void ADrawingHandler::regenerateStroke(FVector position, FRotator rotation, FVector direction)
{
	UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));
	StrokeArray.Last().mesh->Update(position, rotation, direction, brushinfo->size, brushinfo->color);
}

void ADrawingHandler::EraseStroke()
{
}

void ADrawingHandler::BrushsizeUp()
{
	brushinfo->size += 0.5f;
	UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);

}

void ADrawingHandler::BrushsizeDown()
{
}

void ADrawingHandler::ChangeColorG()
{
	brushinfo->color = FLinearColor::Green;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}

void ADrawingHandler::ChangeColorB()
{
	brushinfo->color = FLinearColor::Blue;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}

void ADrawingHandler::ChangeColorR()
{
	brushinfo->color = FLinearColor::Red;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}

// Called when the game starts or when spawned
void ADrawingHandler::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : I'm Drawing handler"));
	EnableInput(GetWorld()->GetFirstPlayerController());
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADrawingHandler::ChangeColorR);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADrawingHandler::ChangeColorG);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADrawingHandler::ChangeColorB);

}

// Called every frame
void ADrawingHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	dt += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%f"), dt);
}

/*void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

    // Respond when our "Grow" key is pressed or released.
    InputComponent->BindAction("Grow", IE_Pressed, this, &AMyPawn::StartGrowing);
    InputComponent->BindAction("Grow", IE_Released, this, &AMyPawn::StopGrowing);

    // Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
    InputComponent->BindAxis("MoveX", this, &AMyPawn::Move_XAxis);
    InputComponent->BindAxis("MoveY", this, &AMyPawn::Move_YAxis);

}*/