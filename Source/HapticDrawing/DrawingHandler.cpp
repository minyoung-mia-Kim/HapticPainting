// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawingHandler.h"
#include "MyProcedualMesh.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MemoryReader.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

////
#include "ConvertedStaticActor.h"


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
	
	// add additional effects
	BrushArray.Add("Material'/Game/Test/M_Glow.M_Glow'");
	BrushArray.Add("Material'/Game/Test/M_Flow.M_Flow'");

	ViscosityArray.Add(5.0f);
	ViscosityArray.Add(4.0f);
	ViscosityArray.Add(3.0f);
	ViscosityArray.Add(3.5f);
	ViscosityArray.Add(2.5f);
	ViscosityArray.Add(2.5f);


	this->brushinfo = new FBrushInfo(BRUSHSTATE::Draw, 0, ViscosityArray[0], 10.f, FLinearColor::White);
	isHapticMode = false;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);

	currentBrushType = brushinfo->type;
	// mesh component for each material
	meshes.Add(nullptr);
	meshes.Add(nullptr);
	meshes.Add(nullptr);
	meshes.Add(nullptr);
	meshes.Add(nullptr);
	meshes.Add(nullptr);
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
			generateStroke(position, rotation, DrawingDirection, position);
			bFbuttonOff = false;
			// store start position to get distance
			startPosition = position;
		}
		else
		{
			if (dt - prvDt > 0.01 && StrokeArray.Num() > 0)
			{
				PositionArray.Add(position);
				RotationArray.Add(rotation);
				//UE_LOG(LogTemp, Warning, TEXT("Direction X:%f, Y:%f, Z:%f"), DrawingDirection.X, DrawingDirection.Y, DrawingDirection.Z);


				if (FVector::Dist(position, prvPositon) > 0.2)
					extendStroke(position, rotation, DrawingDirection, startPosition);

				prvDt = dt;
				//UE_LOG(LogTemp, Warning, TEXT("clicking!"));
				//endPosition = position;
			}

		}
//	}
	prvPositon = position;

}

void ADrawingHandler::FbuttonOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Off"));

	bFbuttonOff = true;
	
	// when F button is off merge section into one mesh component
	if (!meshes[currentBrushType]->bMerged)
	{
		meshes[currentBrushType]->StoreDegeneratedSection();
		meshes[currentBrushType]->MergeSections();
		meshes[currentBrushType]->bMerged = false;
	}
}

void ADrawingHandler::receivedTriggerOn(FVector position, FRotator rotation, bool hasClicked)
{
	DrawingDirection = FVector(position - prvPositon);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), hasClicked ? TEXT("T") : TEXT("F"));

	if (bFbuttonOff)
	{
		generateStroke(position, rotation, DrawingDirection, position);
		bFbuttonOff = false;
		// store start position to get distance
		startPosition = position;
	}
	else
	{
		/* Extend */
		if (dt - prvDt > 0.01 && StrokeArray.Num() > 0)
		{
			PositionArray.Add(position);
			RotationArray.Add(rotation);


			if (FVector::Dist(position, prvPositon) > 0.3) //Default value = 0.2
				extendStroke(position, rotation, DrawingDirection, startPosition);

			prvDt = dt;
		}
	}
	prvPositon = position;

}

void ADrawingHandler::receivedTriggerOff()
{
	UE_LOG(LogTemp, Warning, TEXT("Off"));

	bFbuttonOff = true;

	// when F button is off merge section into one mesh component
	if (!meshes[currentBrushType]->bMerged)
	{
		meshes[currentBrushType]->StoreDegeneratedSection();
		meshes[currentBrushType]->MergeSections();
		meshes[currentBrushType]->bMerged = false;
	}
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

void ADrawingHandler::generateStroke(FVector position, FRotator rotation, FVector direction, FVector startPosition)
{
	previousBrushType.Add(currentBrushType);
	currentBrushType = brushinfo->type;

	UE_LOG(LogTemp, Warning, TEXT("previousBrushType[%d] : %d"), previousBrushType.Num() - 1, previousBrushType.Last());

	if(meshes[currentBrushType])
	{
		if(!meshes[currentBrushType]->bMerged)
		{
			meshes[currentBrushType]->DegenerateSection(position, rotation, direction, brushinfo->size, brushinfo->color, startPosition);
		}
	}

	if(!meshes[currentBrushType])
	{
		meshes[currentBrushType] = GetWorld()->SpawnActor<AProceduralPlaneMesh>(AProceduralPlaneMesh::StaticClass());
		meshes[currentBrushType]->Initialize(position, rotation, direction, brushinfo->size, brushinfo->color, BrushArray[brushinfo->type]);
		StrokeArray.Add(FStroke(meshes[currentBrushType]));
	}
	else
	{
		meshes[currentBrushType]->Update(position, rotation, direction, brushinfo->size, brushinfo->color, startPosition);
	}
}

void ADrawingHandler::extendStroke(FVector position, FRotator rotation, FVector direction, FVector startPosition)
{
	//UE_LOG(LogTemp, Warning, TEXT("re! draw mesh"));

	if (meshes[currentBrushType])
		meshes[currentBrushType]->Update(position, rotation, direction, brushinfo->size, brushinfo->color, startPosition);
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
	if (val == 1 && brushinfo->size > 0.5f)
	{
		brushinfo->size -= 0.1f;
		//UE_LOG(LogTemp, Warning, TEXT("size: %f"), brushinfo->size);
		FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);

	}
}

void ADrawingHandler::UndoStroke()
{
		if (meshes[currentBrushType])
		{
			meshes[currentBrushType]->UndoSection();
			
			if (meshes[currentBrushType]->TotalVertice.Num() == 0)
			{
				meshes[currentBrushType]->Destroy();
				meshes.RemoveAt(currentBrushType);
				meshes.Insert(nullptr, currentBrushType);
				UE_LOG(LogTemp, Warning, TEXT("meshes num : %d"), meshes.Num());
			}
		}

		if (previousBrushType.Num() != 0)
		{
			currentBrushType = previousBrushType.Last();
			previousBrushType.RemoveAt(previousBrushType.Num() - 1);
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
	//InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADrawingHandler::ChangeColorR);
	//InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADrawingHandler::ChangeColorG);
	//InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADrawingHandler::ChangeColorB);

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
	InputComponent->BindKey(EKeys::O, IE_Pressed, this, &ADrawingHandler::SaveToObj);
	//Replay
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &ADrawingHandler::ReplayPainting);

	// Effect Material
	InputComponent->BindKey(EKeys::G, IE_Pressed, this, &ADrawingHandler::EffectMaterialGlow);
	InputComponent->BindKey(EKeys::F, IE_Pressed, this, &ADrawingHandler::EffectMaterialFlow);

	// Mesh change
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &ADrawingHandler::Mesh1);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ADrawingHandler::Mesh2);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ADrawingHandler::Mesh3);
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ADrawingHandler::Mesh4);
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

	if (!FFileHelper::LoadFileToArray(BinaryData, *FString("p_room_floor_walls_details_bed_end_table_chair2_pots_fin.sav")))
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

void ADrawingHandler::EffectMaterialGlow()
{	
	brushinfo->type = 4;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}

void ADrawingHandler::EffectMaterialFlow()
{
	brushinfo->type = 5;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}

void ADrawingHandler::Mesh1()
{
	brushinfo->type = 0;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}

void ADrawingHandler::Mesh2()
{
	brushinfo->type = 1;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}

void ADrawingHandler::Mesh3()
{
	brushinfo->type = 2;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}

void ADrawingHandler::Mesh4()
{
	brushinfo->type = 3;
	FBrushUpdateDelegate.Broadcast(brushinfo->size, brushinfo->color, brushinfo->viscosity, BrushArray[brushinfo->type]);
}
/* save the points of the meshs - XYZ values, RGB values */
void ADrawingHandler::SavePointCloud() {

	/* save the points XYZ values from the meshs */
	FString PointData = FString();
	int NumOfvertice = 0;
	for (auto stroke : StrokeArray) {
		int i = stroke.mesh->vertices.Num();
		/* If there is color for each vertex */
		if (i == stroke.mesh->vertexColors.Num())
		{
			for (i = i - 1; i > 0; i--) {
				// add point one				
				PointData.Append("v " + ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0) + " "); // position
				PointData.Append(ComputeInternalDivisionPointColor(stroke.mesh->vertexColors[i], stroke.mesh->vertexColors[i - 1], 0) + "\n"); //color
				NumOfvertice++;

				// add the middle point of two sequential points
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.5) + " "); // position
				PointData.Append(ComputeInternalDivisionPointColor(stroke.mesh->vertexColors[i], stroke.mesh->vertexColors[i - 1], 0.5) + "\n"); //color
				NumOfvertice++;

				// add the 1:3 internal division point of two sequential points 
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.25) + " "); // position
				PointData.Append(ComputeInternalDivisionPointColor(stroke.mesh->vertexColors[i], stroke.mesh->vertexColors[i - 1], 0.25) + "\n"); //color
				NumOfvertice++;

				// add the 3:1 internal division point of two sequential points
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.75) + " "); // position
				PointData.Append(ComputeInternalDivisionPointColor(stroke.mesh->vertexColors[i], stroke.mesh->vertexColors[i - 1], 0.75) + "\n"); //color
				NumOfvertice++;
			}
		}
		else /* If Not, just add position info */
		{
			for (i = i - 1; i >= 0; i--) {
				NumOfvertice++;
				// add point one				
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0) + " "); // position
				NumOfvertice++;

				// add the middle point of two sequential points
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.5) + " "); // position
				NumOfvertice++;

				// add the 1:3 internal division point of two sequential points 
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.25) + " "); // position
				NumOfvertice++;

				// add the 3:1 internal division point of two sequential points
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0.75) + " "); // position
				NumOfvertice++;
			}
		}

		/* Vertex Normal */

		/* Faces */

	}
	UE_LOG(LogTemp, Warning, TEXT("There are %i vertice in the scene"), NumOfvertice);

	/**	set the name and format of the file,
	*	the format should be .txt to read color values in Meshlab */
	FString PointFileName = FString(TEXT("PointCloud_"));
	PointFileName.Append(FDateTime::Now().ToString());
	PointFileName.AppendChars(TEXT(".txt"), 4);

	/* save the point cloud files */
	if (FFileHelper::SaveStringToFile(PointData, *PointFileName))
	{
		UE_LOG(LogTemp, Warning, TEXT("File save Success. - %s - %s"), FPlatformProcess::BaseDir(), *PointFileName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File save Failed. - PointCloud"));
	}
}


/* save the points of the meshs - XYZ values, RGB values */
void ADrawingHandler::SaveToObj() {


	/* save the points XYZ values from the meshs */
	FString PointData = FString();
	FString FaceData = FString();

	int NumOfvertice = 0;
	for (auto stroke : StrokeArray) {

		stroke.mesh->ConvertProceduralMeshToStaticMesh();

		int i = stroke.mesh->vertices.Num();
		/* If there is color for each vertex */
		if (i == stroke.mesh->vertexColors.Num())
		{

			for (i = i - 1; i > 0; i--) {
				// add point one				
				PointData.Append("v " + ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0) + " "); // position
				PointData.Append(ComputeInternalDivisionPointColor(stroke.mesh->vertexColors[i], stroke.mesh->vertexColors[i - 1], 0) + "\n"); //color
				NumOfvertice++;
			}
		}
		else /* If Not, just add position info */
		{
			for (i = i - 1; i >= 0; i--) {
				NumOfvertice++;
				// add point one				
				PointData.Append(ComputeInternalDivisionPoint(stroke.mesh->vertices[i], stroke.mesh->vertices[i - 1], 0) + " "); // position
				NumOfvertice++;

			}
		}

		/* Vertex Normal */
		i = stroke.mesh->normals.Num();

		for (i = i - 1; i > 0; i--) {
			// add point one				
			PointData.Append("vn " + ComputeInternalDivisionPoint(stroke.mesh->normals[i], stroke.mesh->normals[i - 1], 0) + " "); // position
		}

		/* Faces */
		TArray<int32> IndexBuffer = stroke.mesh->GetProcIndexbuffer();
		FaceData.Append("f ");
		for (int i = 0; i < IndexBuffer.Num()/3; i+=2) {
			int Linebreak = 4;
			// add point one				
			FaceData.Append(FString::FromInt(IndexBuffer[i]) + "/" + 
							FString::FromInt(IndexBuffer[i+1]) + "/" +
							FString::FromInt(IndexBuffer[i+2]) + " "); // faces
			Linebreak--;
			if (Linebreak)
			{
				FaceData.Append("\n"); Linebreak = 4; FaceData.Append("f ");
			}
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("There are %i vertice in the scene"), NumOfvertice);

	/**	set the name and format of the file,
	*	the format should be .txt to read color values in Meshlab */
	FString PointFileName = FString(TEXT("Painting_Room_"));
	PointFileName.Append(FDateTime::Now().ToString());
	PointFileName.AppendChars(TEXT(".obj"), 4);

	/* save the point cloud files */
	if (FFileHelper::SaveStringToFile(PointData+FaceData, *PointFileName))
	{
		UE_LOG(LogTemp, Warning, TEXT("File save Success. - %s - %s"), FPlatformProcess::BaseDir(), *PointFileName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("File save Failed. - SaveToObj"));
	}
}


/* Compute internal Divition Point position XYZ :: check : 0 <= ratio <= 1
 * Result String: "X Y Z"
 * Used In SavePointCloud() */
FString ADrawingHandler::ComputeInternalDivisionPoint(FVector p1, FVector p2, float ratio) {
	FString Result = FString();
	FVector tempVector = p1 * (1 - ratio) + p2 * ratio;

	Result.Append(FString::SanitizeFloat(tempVector.X) + " ");
	Result.Append(FString::SanitizeFloat(tempVector.Y) + " ");
	Result.Append(FString::SanitizeFloat(tempVector.Z));
	return Result;
}

/* Compute internal Divition Point Color RGB :: check : 0 <= ratio <= 1
 * Result String: "R G B"
 * Used In SavePointCloud() */
FString ADrawingHandler::ComputeInternalDivisionPointColor(FLinearColor c1, FLinearColor c2, float ratio) {
	FString Result = FString();
	FLinearColor tempColor = c1 * (1 - ratio) + c2 * ratio;

	Result.Append(FString::SanitizeFloat(tempColor.R) + " ");
	Result.Append(FString::SanitizeFloat(tempColor.G) + " ");
	Result.Append(FString::SanitizeFloat(tempColor.B));
	return Result;
}
