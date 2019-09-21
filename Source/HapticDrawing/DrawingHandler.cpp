// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MemoryReader.h"
#include "Engine/World.h"
#include "Engine/Engine.h"



// Sets default values
ADrawingHandler::ADrawingHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	prvPositon = FVector(0, 0, 0);


	// Add the brush list (Materials)
	BrushArray.Add("Material'/Game/HDAssets/M_grunge1.M_grunge1'");
	BrushArray.Add("Material'/Game/HDAssets/M_BasicBrush.M_BasicBrush'");
	BrushArray.Add("Material'/Game/HDAssets/M_silky1.M_silky1'");
	BrushArray.Add("Material'/Game/HDAssets/M_silky2.M_silky2'");
	ViscosityArray.Add(5.0f);
	ViscosityArray.Add(4.0f);
	ViscosityArray.Add(3.0f);
	ViscosityArray.Add(3.5f);

	this->brushinfo = new FBrushInfo(BRUSHSTATE::Draw, 0, ViscosityArray[0], 10.f, FLinearColor::White);
	isHapticMode = false;


}

void ADrawingHandler::receivedFbutton(FVector position, FRotator rotation, bool hasClicked)
{
	DrawingDirection = FVector(position - prvPositon);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), bFbuttonOff ? TEXT("Off") : TEXT("On"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), hasClicked ? TEXT("T") : TEXT("F"));


	//if (!(DrawingDirection.Size() < 0.5))
	//{
		if (!hasClicked && bFbuttonOff)
		{
			generateStroke(position, rotation, DrawingDirection);
			bFbuttonOff = false;
		}
		else
		{
			if (dt - prvDt > 0.01 && StrokeArray.Num() > 0)
			{
				PositionArray.Add(position);
				RotationArray.Add(rotation);
				//UE_LOG(LogTemp, Warning, TEXT("Direction X:%f, Y:%f, Z:%f"), DrawingDirection.X, DrawingDirection.Y, DrawingDirection.Z);

				if (FVector::Dist(position, prvPositon)> 0.1)
					extendStroke(position, rotation, DrawingDirection);

				prvDt = dt;
				//UE_LOG(LogTemp, Warning, TEXT("clicking!"));
			}

		}
//	}
	prvPositon = position;

}

void ADrawingHandler::FbuttonOff()
{
	bFbuttonOff = true;
	//UE_LOG(LogTemp, Warning, TEXT("%s"), bFbuttonOff ? TEXT("Off") : TEXT("On"));
}

void ADrawingHandler::receivedSbutton(FVector position, FRotator rotation, bool hasClicked)
{
	//UE_LOG(LogTemp, Warning, TEXT("Haptic mode ON"));


	if (!hasClicked)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ddd"));
		//UndoStroke();
	}
}

void ADrawingHandler::generateStroke(FVector position, FRotator rotation, FVector direction)
{
	if (StrokeArray.Num() > 0)
	{
		if (!StrokeArray.Last().mesh->bMerged)
		{
			StrokeArray.Last().mesh->MergeSections();
		}

	}
	//UE_LOG(LogTemp, Warning, TEXT("generate Stroke"));
	AProceduralPlaneMesh* mesh = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
	StrokeArray.Add(FStroke(mesh));
	mesh->Initialize(position, rotation, direction, brushinfo->size, brushinfo->color, BrushArray[brushinfo->type]);
	UE_LOG(LogTemp, Warning, TEXT("In array: %d"), StrokeArray.Num());

}

void ADrawingHandler::extendStroke(FVector position, FRotator rotation, FVector direction)
{
	//UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));
	if(!StrokeArray.Last().mesh->bMerged)
		StrokeArray.Last().mesh->Update(position, rotation, direction, brushinfo->size, brushinfo->color);

}
//template<char key>
void ADrawingHandler::ChangeBrushMode(int tex)
{
	//UE_LOG(LogTemp, Warning, TEXT("grip"));
	brushinfo->type = tex;
	//if (brushinfo->type == BrushArray.Num() - 1)
	//{
	//	brushinfo->type = 0;
	//}
	//else
	//{
	//	brushinfo->type += 1;
	//}
	brushinfo->viscosity = ViscosityArray[brushinfo->type];
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);

}


void ADrawingHandler::BrushsizeUp(float val)
{
	if (val == 1 && brushinfo->size < 30.0f)
	{
		brushinfo->size += 0.1f;
		//UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
		FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);

	}

}

void ADrawingHandler::BrushsizeDown(float val)
{
	if (val == 1 && brushinfo->size > 1.0f)
	{
		brushinfo->size -= 0.1f;
		//UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
		FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);

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
void ADrawingHandler::ChangeColorG()
{
	brushinfo->color = FLinearColor::Green;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}

void ADrawingHandler::ChangeColorB()
{
	brushinfo->color = FLinearColor::Blue;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}


void ADrawingHandler::ChangeColorR()
{
	brushinfo->color = FLinearColor::Red;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushinfo->color.ToString()));

}

void ADrawingHandler::SetBrushColor(FLinearColor sColor, float sSize)
{
	brushinfo->color = sColor;
	brushinfo->size = sSize;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
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

	//InputComponent->BindKey(EKeys::Equals, IE_Pressed, this, &ADrawingHandler::BrushsizeUp);
	//InputComponent->BindKey(EKeys::Hyphen, IE_Pressed, this, &ADrawingHandler::BrushsizeDown);

	//InputComponent->BindKey(EKeys::E, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'E'>);
	//InputComponent->BindKey(EKeys::D, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'D'>);
	//InputComponent->BindKey(EKeys::Z, IE_Pressed, this, &ADrawingHandler::UndoStroke);
/*
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'1'>);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'2'>);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'3'>);
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'4'>);
	InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ADrawingHandler::ChangeBrushMode<'5'>);*/

	InputComponent->BindAction("Undo", IE_Pressed, this, &ADrawingHandler::UndoStroke);
	//InputComponent->BindAction("ChangeBrush", IE_Pressed, this, &ADrawingHandler::ChangeBrushMode);

	InputComponent->BindAxis("BrushSizeUp", this, &ADrawingHandler::BrushsizeUp);
	InputComponent->BindAxis("BrushSizeDown", this, &ADrawingHandler::BrushsizeDown);

	//Save and load
	InputComponent->BindKey(EKeys::S, IE_Pressed, this, &ADrawingHandler::ActorSaveDataSaved);
	InputComponent->BindKey(EKeys::L, IE_Pressed, this, &ADrawingHandler::ActorSaveDataLoaded);
	//Replay
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ADrawingHandler::ReplayPainting);



}

// Called every frame
void ADrawingHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	dt += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%f"), dt);

	if (bReplay)
	{
		if (dt - RprevDt > 0.05)
		{
			UE_LOG(LogTemp, Warning, TEXT("RR"));

			StrokeArray[index].mesh->SetHidden(false);
			index++;
			RprevDt = dt;
		}
		if (index >= StrokeArray.Num() - 1)
		{
			bReplay = false;
			index = 0;
		}

	}
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

void ADrawingHandler::ActorSaveDataSaved()
{
	/*FString MyPlayerName = TEXT("PlayerOne");
	USaveNLoadHandler* SaveGameInstance = Cast<USaveNLoadHandler>(UGameplayStatics::CreateSaveGameObject(USaveNLoadHandler::StaticClass()));
	SaveGameInstance->PlayerName = MyPlayerName;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
*/
///////////////*INTERFACE*////////////////////
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveableActorInterface::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		//Saving each actor to record
		FActorSaveData ActorRecord;
		ActorRecord.ActorName = FName(*Actor->GetName());
		ActorRecord.ActorClass = Actor->GetClass()->GetPathName();
		ActorRecord.ActorTransform = Actor->GetTransform();
		ActorRecord.ProcMeshSections = Cast<AProceduralPlaneMesh>(Actor)->getAllMeshsections();

		FMemoryWriter MemoryWriter(ActorRecord.ActorData, true);
		FSaveGameArchive Ar(MemoryWriter);
		Actor->Serialize(Ar);

		SavedActors.Add(ActorRecord);
		ISaveableActorInterface::Execute_ActorSaveDataSaved(Actor);
	}

	FSaveGameData SaveGameData;

	SaveGameData.GameID = "1234";
	SaveGameData.Timestamp = FDateTime::Now();
	SaveGameData.SavedActors = SavedActors;

	FBufferArchive BinaryData;

	BinaryData << SaveGameData;
	FString FileName = FString(TEXT("TestSave"));
	FileName.Append(SaveGameData.Timestamp.ToString());
	FileName.AppendChars(TEXT(".sav"), 4);

	//FString AbsoluteFilePath = "C:\Users\glab\Documents\00Project\HapticDrawing";
	if (FFileHelper::SaveArrayToFile(BinaryData, *FileName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Success! %s"), FPlatformProcess::BaseDir());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save Failed!"));
	}

	BinaryData.FlushCache();
	BinaryData.Empty();
}



void ADrawingHandler::ActorSaveDataLoaded()
{

	//USaveNLoadHandler* LoadGameInstance = Cast<USaveNLoadHandler>(UGameplayStatics::CreateSaveGameObject(USaveNLoadHandler::StaticClass()));
	//LoadGameInstance = Cast<USaveNLoadHandler>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
	//FString PlayerNameToDisplay = LoadGameInstance->PlayerName;
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, PlayerNameToDisplay);
	//}


	int TotalVerticeNum = 0;
	TArray<uint8> BinaryData;
	UE_LOG(LogTemp, Warning, TEXT("Loading"));

	if (!FFileHelper::LoadFileToArray(BinaryData, *FString("TestSave2019.09.09-20.57.06.sav")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Failed!"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Load Succeeded!"));
	}


	FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
	FromBinary.Seek(0);

	FSaveGameData SaveGameData;
	FromBinary << SaveGameData;

	FromBinary.FlushCache();
	BinaryData.Empty();
	FromBinary.Close();

	Loaded(SaveGameData);


}

void ADrawingHandler::Loaded(FSaveGameData SaveGameData)
{
	int TotalVerticeNum = 0;
	for (FActorSaveData ActorRecord : SaveGameData.SavedActors)
	{
		FVector SpawnPos = ActorRecord.ActorTransform.GetLocation();
		FRotator SpawnRot = ActorRecord.ActorTransform.Rotator();

		/* array */
		PositionArray.Add(SpawnPos);
		RotationArray.Add(SpawnRot);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Name = ActorRecord.ActorName;
		UClass* SpawnClass = FindObject<UClass>(ANY_PACKAGE, *ActorRecord.ActorClass);
		if (SpawnClass)
		{
			AActor* NewActor = GWorld->SpawnActor(SpawnClass, &SpawnPos, &SpawnRot, SpawnParams);
			FMemoryReader MemoryReader(ActorRecord.ActorData, true);
			FSaveGameArchive Ar(MemoryReader);
			NewActor->Serialize(Ar);
			NewActor->SetActorTransform(ActorRecord.ActorTransform);
			Cast<AProceduralPlaneMesh>(NewActor)->LoadMeshsections(ActorRecord.ProcMeshSections);
			TotalVerticeNum += ActorRecord.ProcMeshSections.vertices.Num();
			ISaveableActorInterface::Execute_ActorSaveDataLoaded(NewActor);
			StrokeArray.Add(FStroke(Cast<AProceduralPlaneMesh>(NewActor)));
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("# Actors: %d "), SaveGameData.SavedActors.Num());
	UE_LOG(LogTemp, Warning, TEXT("# Vertices: %d "), TotalVerticeNum);
}

void ADrawingHandler::ReplayPainting()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReplayPainting"));

	for (FStroke& Stroke : StrokeArray)
	{
		Stroke.mesh->SetHidden(true);
	}
	//TimerDel.BindUFunction(this, FName("Loaded"));
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADrawingHandler::ShowPainting , 0.05f, true);
	bReplay = true;
}

void ADrawingHandler::ShowPainting()
{
	int i = 0;
	float prevDt = 0.0f;
	while (i < StrokeArray.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("dt %f"), dt);


	}



	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("# Actors: %f "), SaveGameData.SavedActors.Num()));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("# Vertice"), ArrMeshesections.vertices.Num()));

}
