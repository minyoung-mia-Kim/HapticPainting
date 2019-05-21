// Fill out your copyright notice in the Description page of Project Settings.



#include "HapticHandler.h"
#include "IHaptico.h"
#include "AsyncWork.h"
#include "HapticThread.h"
#include "HapticThreadInput.h"
#include "HapticThreadOutput.h"
#include "MainController.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

/**
 * constructs an instance of the haptic manager
*/
AHapticsHandler::AHapticsHandler()
{
	PrimaryActorTick.bCanEverTick = true;
	noHapticDevice = false;
	rc = CreateDefaultSubobject<USceneComponent>(TEXT("Transform"));
	//rc->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	//SetRootComponent(rc);

	cursor = CreateDefaultSubobject<USphereComponent>(TEXT("Cursor"));
	// set up a notification for when this component overlaps something
	//cursor->SetNotifyRigidBodyCollision(true);
	//cursor->BodyInstance.SetCollisionProfileName("BlockAll");
	//cursor->OnComponentHit.AddDynamic(this, &AHapticsHandler::OnHit);

	cursor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	cursor->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	cursor->bHiddenInGame = false;
	SetRootComponent(cursor);

	brush = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	brush->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//brush->SetWorldLocation(FVector(cursor->GetScaledSphereRadius(), 0.0f, 0.0f));
	brush->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	CreateBrushCursor(10.0f, FLinearColor::White);

	cursor->OnComponentBeginOverlap.AddDynamic(this, &AHapticsHandler::OnComponentBeginOverlap);
	cursor->OnComponentEndOverlap.AddDynamic(this, &AHapticsHandler::OnComponentEndOverlap);
	cursor->SetEnableGravity(false);
	cursor->SetSimulatePhysics(false);
}

/**
 * Called when the actor is spawned and starts the haptic thread
 */
void AHapticsHandler::BeginPlay()
{
	Super::BeginPlay();
	UHapticThreadInput::getInst().setRunThread(true);
	(new FAutoDeleteAsyncTask<FHapticThread>(IHaptico::Get(), this))->StartBackgroundTask();
	//if (!IHaptico::connect)
	//	noHapticDevice = true;
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay : I'm handler"));
	/* Haptic status */
	hasFBClicked = false;
	hasSBClicked = false;
	isOverlapping = false;
	CurrentForce = FVector::ZeroVector;
}

/**
* Called when the actor is destroyed and ends the haptic thread
*/
void  AHapticsHandler::EndPlay(EEndPlayReason::Type type)
{
	Super::EndPlay(type);
	UHapticThreadInput::getInst().setRunThread(false);
}

/**
 * Called every frame
*/
void AHapticsHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	hasFBClicked = BHandler->button1AlreadyPressed;
	hasSBClicked = BHandler->button2AlreadyPressed;

	/* Brush Normal and Tangent */
	FVector MyLocation = this->GetActorLocation();
	FRotator MyRotation = this->GetActorRotation();
	FVector Direction = -MyRotation.Vector();
	Direction.Normalize();
	DrawDebugLine(GetWorld(), brush->GetComponentLocation(), brush->GetComponentLocation() + Direction * 10.0f, FColor::Red, false, 0, 0, 0.5);

	
	FVector v1 = FVector(MyLocation.X, MyLocation.Y, MyLocation.Z) + MyRotation.RotateVector(FVector(0.0f, 0.0f, 5.0f));
	FVector v2 = FVector(MyLocation.X, MyLocation.Y, MyLocation.Z) + MyRotation.RotateVector(FVector(0.0f, 0.0f, -5.0f));
	
	FVector surfaceTangent = v1- v2;
	surfaceTangent = surfaceTangent.GetSafeNormal();

	if (surfaceTangent == FVector().ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("vector length is too small to safely normalize"));

	DrawDebugLine(GetWorld(), brush->GetComponentLocation(), brush->GetComponentLocation()+ surfaceTangent * 5.0f, FColor::Blue, false, 0, 0, 0.5);

	for (int i = 0; i < brush->GetProcMeshSection(0)->ProcVertexBuffer.Num(); i++)
	{
		brush->GetProcMeshSection(0)->ProcVertexBuffer[i].Normal = Direction;
		brush->GetProcMeshSection(0)->ProcVertexBuffer[i].Tangent = FProcMeshTangent(surfaceTangent, true);
	}

	/* status */
	if (isOverlapping)
	{
		setForceToApply(FMath::Lerp(FVector(0.0f, 0.0f, 0.0f), CurrentForce, 0.1));
		if (hasFBClicked)
		{
			setForceToApply(FMath::Lerp(CurrentForce, FVector(0.0f, 0.0f, 0.0f), 0.1));
		}
	}
	else
	{
		setForceToApply(FMath::Lerp(CurrentForce, FVector(0.0f, 0.0f, 0.0f), 0.1));
	}

	if (CurrentForce == FVector::ZeroVector)
		isOverlapping = false;
}

/**
 * set the force that should be applied to the haptic device in the next tick of the haptic thread
*/
void AHapticsHandler::setForceToApply(FVector force) {
	UHapticThreadInput::getInst().setForceToApply(force);
}

/**
* set the torque that should be applied to the haptic device in the next tick of the haptic thread
*/
void AHapticsHandler::setTorqueToApply(FVector torque) {
	UHapticThreadInput::getInst().setTorqueToApply(torque);
}

/**
* gets the current position of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDevicePosition() {
	return UHapticThreadOutput::getInst().getHapticCursorPosition();
}

/**
* get the current linear velocity of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDeviceLinearVelocity() {
	return UHapticThreadOutput::getInst().getHapticCursorLinearVelocity();
}

/**
* get the current angular velocity of the haptic device end affector
*/
FVector AHapticsHandler::getHapticDeviceAngularVelocity() {
	return UHapticThreadOutput::getInst().getHapticCursorAngularVelocity();
}

/**
* get the current rotation of the haptic device end affector
*/
FMatrix AHapticsHandler::getHapticDeviceRotation() {
	return UHapticThreadOutput::getInst().getHapticCursorRotation();
}

/**
* get the current rotation of the haptic device end affector as an unreal rotator
*/
FRotator AHapticsHandler::getHapticDeviceRotationAsUnrealRotator() {
	FMatrix rotation = UHapticThreadOutput::getInst().getHapticCursorRotation();
	FVector euler = rotation.Rotator().Euler();
	//Re-adjusted the angle
	//Pitch(Updown) : the same with the stylus pitch, Yaw(RL), Roll(CW, CCW)
	return FRotator(-euler.Y, -euler.Z + 180.f, euler.X);
}

/**
* get the current position of the haptic device end affector in unreal coordinates
*/
FVector AHapticsHandler::getHapticDevicePositionInUnrealCoordinates() {
	FVector position = UHapticThreadOutput::getInst().getHapticCursorPosition();
	//Re-adjusted the position
	return FVector(position.X * 1000, -position.Y * 1000, (position.Z * 1000 + 65.0f));
}

/**
* broad casts the new haptic data as a multicast delegate
*/
void AHapticsHandler::broadCastNewHapticData(FVector position, FMatrix rotation, FVector linearVelocity, FVector angularVelocity) {
	OnHapticHandlerTick.Broadcast(position, rotation, linearVelocity, angularVelocity);
}
void AHapticsHandler::button1Clicked()
{
	FVector position = this->getHapticDevicePositionInUnrealCoordinates();
	//UE_LOG(LogTemp, Warning, TEXT("I'm handler b1 clicked"));

	FbuttonInputDelegate.Broadcast(position, hasFBClicked);

	//if(BHandler->button1AlreadyPressed)
	//	UE_LOG(LogTemp, Warning, TEXT("I'm handler b1 already clicked"));
}

void AHapticsHandler::button2Clicked()
{
	FVector position = this->getHapticDevicePositionInUnrealCoordinates();
	//UE_LOG(LogTemp, Warning, TEXT("I'm handler b2 clicked"));
	SbuttonInputDelegate.Broadcast(position, hasSBClicked);
}
/*
* Collistion event
*/
void AHapticsHandler::OnComponentBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	isOverlapping = true;

	if (!hasFBClicked)
	{
		FVector brushNormal = brush->GetProcMeshSection(0)->ProcVertexBuffer[0].Normal;
		FVector brushTangent = brush->GetProcMeshSection(0)->ProcVertexBuffer[0].Tangent.TangentX;

		//UE_LOG(LogTemp, Warning, TEXT("Overlapped"));
		//UE_LOG(LogTemp, Warning, TEXT("brush Normal X:%f, Y:%f, Z:%f"), brushNormal.X, brushNormal.Y, brushNormal.Z);
		//UE_LOG(LogTemp, Warning, TEXT("brush Normal X:%f, Y:%f, Z:%f"), -);


		//UE_LOG(LogTemp, Warning, TEXT("OverlappedComponent : %s"), *(OverlappedComp->GetName()));
		//UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *(OtherComp->GetName()));
		//UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *(SweepResult.GetComponent()->GetName()));
		////UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), (bFromSweep ? TEXT("True") : TEXT("False")));

		UProceduralMeshComponent* detectMesh = Cast<UProceduralMeshComponent>(OtherComp);
		if (detectMesh != nullptr)
		{
			for (int i = 0; i < detectMesh->GetNumSections(); i++)
			{
				FProcMeshSection* ms = detectMesh->GetProcMeshSection(i);
				//UE_LOG(LogTemp, Warning, TEXT("buffer Normal X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[0].Normal.X, ms->ProcVertexBuffer[0].Normal.Y, ms->ProcVertexBuffer[0].Normal.Z);

				if (ms->ProcVertexBuffer[0].Normal.Equals(brushNormal, 0.1f) &&
					ms->ProcVertexBuffer[0].Tangent.TangentX.Equals(brushTangent, 0.1f))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Found it"));

					/* Debug */
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Overlapped")));
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("brush Normal X:%f, Y:%f, Z:%f"), brushNormal.X, brushNormal.Y, brushNormal.Z));
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("brush Tangent X:%f, Y:%f, Z:%f"), brushTangent.X, brushTangent.Y, brushTangent.Z));
					//UE_LOG(LogTemp, Warning, TEXT("Tangent X:%f, Y:%f, Z:%f"), ms->ProcVertexBuffer[0].Tangent.TangentX.X, ms->ProcVertexBuffer[0].Tangent.TangentX.Y, ms->ProcVertexBuffer[0].Tangent.TangentX.Z);

					FHitResult OutHit;
					FVector cursorTip = FVector(OverlappedComp->GetComponentLocation().X + cursor->GetScaledSphereRadius(), OverlappedComp->GetComponentLocation().Y, OverlappedComp->GetComponentLocation().Z);
					FVector Start = cursorTip;

					FVector ForwardVector = OverlappedComp->GetForwardVector();
					FVector End = ((ForwardVector * 1000.f) + Start);
					FCollisionQueryParams CollisionParams;

					DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

					if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
					{
							GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
							GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
							GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
							UE_LOG(LogTemp, Warning, TEXT("hitting %s"), *OutHit.GetActor()->GetName());
							UE_LOG(LogTemp, Warning, TEXT("Impact %s"), *OutHit.ImpactPoint.ToString());
							UE_LOG(LogTemp, Warning, TEXT("Normal %s"), *CurrentForce.ToString());
					}

					float dist = FVector::PointPlaneDist(cursorTip, ms->ProcVertexBuffer[0].Position, ms->ProcVertexBuffer[0].Normal);
					UE_LOG(LogTemp, Warning, TEXT("distance %f"), dist);
					//float pDepth = FVector::Distance(OverlappedComp->GetComponentLocation(), ms->ProcVertexBuffer[0].Position);
					CurrentForce = cursorTip/dist * (cursor->GetScaledSphereRadius()-dist);
					UE_LOG(LogTemp, Warning, TEXT("Force %s"), *CurrentForce.ToString());

					//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Cyan, FString::Printf(TEXT("Force X:%f, Y:%f, Z:%f"), CurrentForce.X, CurrentForce.Y, brushNormal.Z));


				}

				//if (ms->ProcVertexBuffer[0].Normal.Equals(brushNormal, 0.03f))
				//{
				//	UE_LOG(LogTemp, Warning, TEXT("Found it"));
				//	CurrentForce = ms->ProcVertexBuffer[0].Normal;
				//	setForceToApply(CurrentForce);
				//}
			}
		}

	}

	//UE_LOG(LogTemp, Warning, TEXT("Normal X:%f, Y:%f, Z:%f"), detectMesh->);
	//UE_LOG(LogTemp, Warning, TEXT("bDepth : %d"), SweepResult.bStartPenetrating);

	//UE_LOG(LogTemp, Warning, TEXT("Depth : %f"), SweepResult.PenetrationDepth);
	////DrawDebugSphere(GetWorld(), SweepResult.Location, 16, 32, FColor(255, 0, 0), false, 3.0f);
	//
	//	



}
void AHapticsHandler::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	isOverlapping = false;

	//UE_LOG(LogTemp, Warning, TEXT("Finished"));
	////UE_LOG(LogTemp, Warning, TEXT("OverlappedComponent : %s"), *(OverlappedComp->GetName()));
	////UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *(OtherComp->GetName()));
	if (!hasFBClicked)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Finished")));
		//CurrentForce = FVector(0.0f, 0.0f, 0.0f);
	}
}

/*
	Redraw the cursor by the new brush information
*/

void AHapticsHandler::RefreshBrushCursor(float brushSize, FLinearColor brushColor)
{
	brush->ClearAllMeshSections();
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushColor.ToString()));
	CreateBrushCursor(brushSize, brushColor);
}

void AHapticsHandler::CreateBrushCursor(float brushSize, FLinearColor brushColor)
{
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	int32 height = 2;
	int32 width = 2;
	float spacing = brushSize;
	float uvSpacing = 1.0f / FMath::Max(height, width);

	FString MaterialAddress = "Material'/Game/ArchVis/Materials/M_Carptet_Mat.M_Carptet_Mat'";
	Material = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/M_Color.M_Color'"));

	vertices.Add(FVector(0.0f, 1.0f, spacing / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, 1.0f, -spacing / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, spacing / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, -spacing / 2));
	vertexColors.Add(brushColor); //red


	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			uvs.Add(FVector2D(x * uvSpacing, y * uvSpacing));
			tangents.Add(FProcMeshTangent(0.0f, 1.0f, 0.0f));
		}
	}
	//Front-face
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Add(x + (y * width));					//current vertex
			triangles.Add(x + (y * width) + width);			//current vertex + row
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right

			triangles.Add(x + (y * width));					//current vertex
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right
			triangles.Add(x + (y * width) + 1);				//current vertex + one right
		}
	}
	//Opposite Front-face
	for (int32 y = 0; y < height - 1; y++)
	{
		for (int32 x = 0; x < width - 1; x++)
		{
			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + 1);				//current vertex + one right		: 1
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3

			triangles.Add(x + (y * width));					//current vertex					: 0
			triangles.Add(x + (y * width) + width + 1);		//current vertex + row + one right	: 3
			triangles.Add(x + (y * width) + width);			//current vertex + row				: 2
		}
	}
	brush->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, false);
	brush->SetMaterial(0, Material);

}

void AHapticsHandler::SetCursorRotation(FRotator rotation)
{
	//cursor->AddWorldRotation(rotation);
	//brush->AddWorldRotation(rotation);
}

//void AHapticsHandler::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
//{
//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Hitted(Blocked)"));
//		UE_LOG(LogTemp, Warning, TEXT("Depth : %f"), Hit.PenetrationDepth);
//

//	}
//
//}


