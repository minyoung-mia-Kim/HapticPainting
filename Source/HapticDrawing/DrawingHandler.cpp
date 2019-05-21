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
	prvPositon = FVector(0, 0, 0);

	// Add the brush list (Materials)
	BrushArray.Add("Material'/Game/HDAssets/M_silky1.M_silky1'");
	BrushArray.Add("Material'/Game/HDAssets/M_BasicBrush.M_BasicBrush'");
	BrushArray.Add("Material'/Game/HDAssets/M_silky2.M_silky2'");
	BrushArray.Add("Material'/Game/HDAssets/M_grunge1.M_grunge1'");
	BrushArray.Add("Material'/Game/M_Color.M_Color'");
	BrushArray.Add("Material'/Game/M_NonLight.M_NonLight''");
	BrushArray.Add("Material'/Game/HDAssets/M_Texture_test.M_Texture_test''");
	BrushArray.Add("Material'/Game/HDAssets/M_Texture_test2.M_Texture_test2''");

	this->brushinfo = new FBrushInfo(BRUSHSTATE::Draw, BrushArray[0], 10.f, FLinearColor::White);

}

void ADrawingHandler::receivedFbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//FVector Normal = -(position - FVector(40.f, 0.f, 0.f));
	DrawingDirection = FVector(position - prvPositon);
	if (brushinfo->state == BRUSHSTATE::Draw)
	{
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
				//UE_LOG(LogTemp, Warning, TEXT("Direction X:%f, Y:%f, Z:%f"), DrawingDirection.X, DrawingDirection.Y, DrawingDirection.Z);

				if (FMath::Abs(FVector::Dist(position, prvPositon)))
					regenerateStroke(position, rotation, DrawingDirection);

				prvDt = dt;
				//UE_LOG(LogTemp, Warning, TEXT("clicking!"));
			}

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
		//UE_LOG(LogTemp, Warning, TEXT("ddd"));
		//UndoStroke();
	}
}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation, FVector direction)
{
	//UE_LOG(LogTemp, Warning, TEXT("generate Stroke"));
	AProceduralPlaneMesh* mesh = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	StrokeArray.Add(FStroke(position, position, mesh));
	mesh->Initialize(position, rotation, direction, brushinfo->size, brushinfo->color, brushinfo->mode);
	//UE_LOG(LogTemp, Warning, TEXT("In array: %d"), StrokeArray.Num());

}

void ADrawingHandler::regenerateStroke(FVector position, FRotator rotation, FVector direction)
{
	//UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));
	StrokeArray.Last().mesh->Update(position, rotation, direction, brushinfo->size, brushinfo->color);

}
template<char key>
void ADrawingHandler::ChangeBrushMode()
{
	ChangeBrushMode(key);
}
void ADrawingHandler::ChangeBrushMode(char key)
{
	if (key == 'D')
	{
		UE_LOG(LogTemp, Warning, TEXT("Draw Mode"));
		brushinfo->state = BRUSHSTATE::Draw;

	}
	else if (key == '1')
	{
		brushinfo->mode = BrushArray[0];
	}
	else if (key == '2')
	{
		brushinfo->mode = BrushArray[1];
	}
	else if (key == '3')
	{
		brushinfo->mode = BrushArray[2];
	}
	else if (key == '4')
	{
		brushinfo->mode = BrushArray[3];
	}
	else if (key == '5')
	{
		brushinfo->mode = BrushArray[4];
	}
}

void ADrawingHandler::BrushsizeUp(float val)
{
	if (val == 1 && brushinfo->size < 30.0f)
	{
		brushinfo->size += 0.1f;
		UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
		FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);

	}

}

void ADrawingHandler::BrushsizeDown(float val)
{
	if (val == 1 && brushinfo->size > 1.0f)
	{
		brushinfo->size -= 0.1f;
		UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
		FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);

	}
}

void ADrawingHandler::UndoStroke()
{
	if (StrokeArray.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(">>"));

		StrokeArray.Last().mesh->Destroy();
		StrokeArray.RemoveAt(StrokeArray.Num() - 1);
	}
}

/* Deprecated :: Change color by keyboard input */
//
//void ADrawingHandler::ChangeColorG()
//{
//	brushinfo->color = FLinearColor::Green;
//	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
//	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));
//
//}
//
//void ADrawingHandler::ChangeColorB()
//{
//	brushinfo->color = FLinearColor::Blue;
//	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
//	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));
//
//}
//
//
//void ADrawingHandler::ChangeColorR()
//{
//	brushinfo->color = FLinearColor::Red;
//	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
//	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));
//	
//}

void ADrawingHandler::SetBrushColor(FLinearColor sColor)
{
	brushinfo->color = sColor;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color);
}

// Called when the game starts or when spawned
void ADrawingHandler::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : I'm Drawing handler"));

	EnableInput(GetWorld()->GetFirstPlayerController());
	//InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ADrawingHandler::ChangeColorR);
	//InputComponent->BindKey(EKeys::G, IE_Pressed, this, &ADrawingHandler::ChangeColorG);
	//InputComponent->BindKey(EKeys::B, IE_Pressed, this, &ADrawingHandler::ChangeColorB);

	//InputComponent->BindKey(EKeys::Equals, IE_Pressed, this, &ADrawingHandler::BrushsizeUp);
	//InputComponent->BindKey(EKeys::Hyphen, IE_Pressed, this, &ADrawingHandler::BrushsizeDown);
	
	//InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'E'>);
	//InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'D'>);
	//InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ADrawingHandler::UndoStroke);

	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'1'>);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'2'>);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'3'>);
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'4'>);
	InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'5'>);

	InputComponent->BindAction("Undo", IE_Pressed, this, &ADrawingHandler::UndoStroke);
	InputComponent->BindAxis("BrushSizeUP", this, &ADrawingHandler::BrushsizeUp);
	InputComponent->BindAxis("BrushSizeDown", this, &ADrawingHandler::BrushsizeDown);



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