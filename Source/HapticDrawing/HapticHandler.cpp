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
#include "ConstructorHelpers.h"


/**
 * constructs an instance of the haptic manager
*/
AHapticsHandler::AHapticsHandler()
{
	PrimaryActorTick.bCanEverTick = true;
	noHapticDevice = false;

	// set up a notification for when this component overlaps something
	//cursor->BodyInstance.SetCollisionProfileName("BlockAll");
	//cursor->OnComponentHit.AddDynamic(this, &AHapticsHandler::OnHit);
	RComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RComponent);

	cursor = CreateDefaultSubobject<USphereComponent>(TEXT("Cursor"));
	cursor->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	cursor->SetNotifyRigidBodyCollision(true);
	cursor->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	cursor->bHiddenInGame = false;
	//SetRootComponent(cursor);

	brush = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	brush->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	brush->SetWorldLocation(FVector(cursor->GetScaledSphereRadius(), 0.0f, 0.0f));
	brush->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	CreateBrushCursor(10.0f, FLinearColor::White, "Material'/Game/HDAssets/M_grunge1.M_grunge1'");

	/* Collision overlap check with storke*/
	cursor->OnComponentBeginOverlap.AddDynamic(this, &AHapticsHandler::OnComponentBeginOverlap);
	cursor->OnComponentEndOverlap.AddDynamic(this, &AHapticsHandler::OnComponentEndOverlap);
	cursor->SetEnableGravity(false);
	cursor->SetSimulatePhysics(false);

	DrawingPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DrawingPlane"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshOb_plane(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMeshOb_plane.Object)
		DrawingPlane->SetStaticMesh(StaticMeshOb_plane.Object);
	DrawingPlane->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DrawingPlane->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	//DrawingPlane->SetWorldRotation(FRotator(90.f, 0.f, 0.f));
	DrawingPlane->SetWorldLocation(FVector(cursor->GetScaledSphereRadius(), 0.0f, 0.0f));
	DrawingPlane->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	UMaterialInterface* PlaneMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/M_Test.M_Test'"));
	DrawingPlane->SetMaterial(0, PlaneMaterial);
	DrawingPlane->SetNotifyRigidBodyCollision(true);
	DrawingPlane->SetEnableGravity(false);
	DrawingPlane->BodyInstance.SetCollisionProfileName("OverlapAll");
	//DrawingPlane->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//cursor->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	/* Collision block check with vdp*/
	//DrawingPlane->OnComponentBeginOverlap.AddDynamic(this, &AHapticsHandler::OnBeginOverlap);

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
	bIsSpringOn = false;
	bIsOnVDP = false;
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
	prvFBstat = hasFBClicked;
	hasFBClicked = BHandler->button1AlreadyPressed;
	hasSBClicked = BHandler->button2AlreadyPressed;

	//UE_LOG(LogTemp, Warning, TEXT("hasFBClicked %s"), hasFBClicked ? TEXT("True") : TEXT("False"));


	/* Button Up = Finish Drawing */
	if (prvFBstat == true && hasFBClicked == false)
	{
		FHapticModeUpdateDelegate.Broadcast();
		//bIsOnVDP = false;
	}

	/* Texture haptic force*/
	if (hasFBClicked)
		setForceToApply(getHapticDeviceLinearVelocity() * -viscosity);
	else
		setForceToApply(FVector::ZeroVector);

	if (hasSBClicked)
	{
		SButtonDt += DeltaTime;
	}
	else
		SButtonDt = 0.0f;

	/* VDP */
	if (bIsOnVDP && !bIsSpringOn)
	{

		FVector Start = cursor->GetComponentLocation();
		FVector End = ((this->GetActorForwardVector() * 1000.f) + Start);
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
		FVector force = FVector::ZeroVector;

		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
		if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, TraceObjectTypes))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetComponent()->GetName()));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("D: %f"), OutHit.Distance));
			//DrawDebugPoint(GetWorld(), brush->GetComponentLocation(), 5.5f, FColor::Magenta, true, 5.f);



			float d = OutHit.Distance;
			float r = cursor->GetScaledSphereRadius();
			float k1 = 3.5f;
			float k2 = 5.5f;
			float dp = FVector(OutHit.Location - brush->GetComponentLocation()).Size();

			FVector n = OutHit.ImpactNormal;
			float vdp = FVector::DotProduct(n, (OutHit.Location - brush->GetComponentLocation()));
			float dmax = -FVector::DotProduct(n, (OutHit.Location - cursor->GetComponentLocation()));

			float lp = FVector::DotProduct(n, FVector(brush->GetComponentLocation() - cursor->GetComponentLocation()));

			//DrawDebugLine(GetWorld(), OutHit.ImpactPoint, OutHit.ImpactPoint + n*5.f, FColor::Cyan, true, 5.f, 0, 1);
			//DrawDebugLine(GetWorld(), Start, Start + OutHit.Location, FColor::Red, true, 5.f, 0, 1);
			UE_LOG(LogTemp, Warning, TEXT("vdp: %f"), vdp);
			UE_LOG(LogTemp, Warning, TEXT("dmax: %f"), dmax);


			//if(dp > r)
			if (vdp <= 0)
			{

				force = FVector::ZeroVector;
				UE_LOG(LogTemp, Warning, TEXT("f : %s"), *(force.ToString()));

				OutHit.Reset();

			}
			//if (r/2 < dp && dp <= r)
			else if (vdp > 0 && vdp <= dmax)
			{
				//force = FVector(n * (k1 / lp) * dp);
				force = FVector(n * k1 * vdp / dmax);

				//setForceToApply(n * 3.5f );

			}
			//else if (dp <= r/2)
			else if (vdp > dmax && vdp > 0)
			{
				//setForceToApply(n * (1.5f + 3.0f * (d - r) / r));
				//force = n * (k1 + (k2/lp) * (dp - lp));

				force = n * (k1 + k2 * (vdp - dmax) / dmax);

			}
			setForceToApply(GetActorRotation().RotateVector(FVector(-force.X, force.Y, force.Z)));
			bforce = force;
		}

	}

	/* Brush Normal and Tangent */

	FVector MyLocation = this->GetActorLocation();
	FRotator MyRotation = this->GetActorRotation();
	FVector Direction = -MyRotation.Vector();
	Direction.Normalize();
	DrawDebugLine(GetWorld(), brush->GetComponentLocation(), brush->GetComponentLocation() + Direction * 10.0f, FColor::Red, false, 0, 0, 0.5);


	FVector v1 = FVector(MyLocation.X, MyLocation.Y, MyLocation.Z) + MyRotation.RotateVector(FVector(0.0f, 0.0f, 5.0f));
	FVector v2 = FVector(MyLocation.X, MyLocation.Y, MyLocation.Z) + MyRotation.RotateVector(FVector(0.0f, 0.0f, -5.0f));

	FVector surfaceTangent = v1 - v2;
	surfaceTangent = surfaceTangent.GetSafeNormal();

	if (surfaceTangent == FVector().ZeroVector)
		UE_LOG(LogTemp, Warning, TEXT("vector length is too small to safely normalize"));

	DrawDebugLine(GetWorld(), brush->GetComponentLocation(), brush->GetComponentLocation() + surfaceTangent * 5.0f, FColor::Blue, false, 0, 0, 0.5);

	for (int i = 0; i < brush->GetProcMeshSection(0)->ProcVertexBuffer.Num(); i++)
	{
		brush->GetProcMeshSection(0)->ProcVertexBuffer[i].Normal = Direction;
		brush->GetProcMeshSection(0)->ProcVertexBuffer[i].Tangent = FProcMeshTangent(surfaceTangent, true);
	}


}
void AHapticsHandler::setViscosity(float v)
{
	this->viscosity = v;
}


/*
* Collistion event
*/
void AHapticsHandler::OnComponentBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/* Check overlappedComp */
	FString overComp = OtherComp->GetName();
	UE_LOG(LogTemp, Warning, TEXT("OverlappedComp : %s"), *(OtherComp->GetName()));

	/* Spring-mass Force */
	if (hasSBClicked && SButtonDt > 0.5f && overComp == "ProceduralMesh")
	{
		bIsOnVDP = false;
		UE_LOG(LogTemp, Warning, TEXT("gotit time %f Clicked %s"), SButtonDt, hasSBClicked ? TEXT("True") : TEXT("False"));
		bIsSpringOn = true;
		FVector brushNormal = brush->GetProcMeshSection(0)->ProcVertexBuffer[0].Normal;
		FVector brushTangent = brush->GetProcMeshSection(0)->ProcVertexBuffer[0].Tangent.TangentX;

		/* Second force rendering */
		AProceduralPlaneMesh* detectStrokeActor = Cast<AProceduralPlaneMesh>(OtherActor);
		UProceduralMeshComponent* detectMesh = Cast<UProceduralMeshComponent>(OtherComp);
		if (detectStrokeActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("mesh sec # %d"), detectMesh->GetNumSections());

			//setForceToApply(FVector::ZeroVector);

			for (int i = 0; i < detectStrokeActor->centerPos.Num(); i++)
			{
				if (OverlappedComp->GetComponentLocation().Equals(detectStrokeActor->centerPos[i], brushSize / 3))
				{
					FVector refinedCPosition = OverlappedComp->GetComponentLocation();
					refinedCPosition.X += cursor->GetScaledSphereRadius();

					FVector centerPosition = detectStrokeActor->centerPos[i];
					DrawDebugPoint(GetWorld(), centerPosition, 10.f, FColor::Green, true, 1, 0);
					HapticCollisionData.Broadcast(detectStrokeActor->centerPos[i],
						detectStrokeActor->centerNormals[i],
						detectMesh->GetProcMeshSection(i)->ProcVertexBuffer[0].Tangent.TangentX,
						refinedCPosition);


					UE_LOG(LogTemp, Warning, TEXT("centorPos %s"), *centerPosition.ToString());
					//UE_LOG(LogTemp, Warning, TEXT("tangent %s"), *centerTangent.ToString());
					//UE_LOG(LogTemp, Warning, TEXT("normal %s"), *detectStrokeActor->centerNormals[i].ToString());
					////UE_LOG(LogTemp, Warning, TEXT("dist %f"), dist);
					////UE_LOG(LogTemp, Warning, TEXT("intensity %f"), intensity * dot);
				}

			}
		}

	}
	///* Intersection Force */
	//if (hasFBClicked)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("intersect with stroke %s"), *(getHapticDeviceLinearVelocity().ToString()));
	//	bIntersected = true;
	//	setForceToApply(getHapticDeviceLinearVelocity() * -5.f);
	//}

	/* Haptic Force : Virtual drawing plane */
	if (overComp == "DrawingPlane")
	{
		UStaticMeshComponent* DPlane = Cast<UStaticMeshComponent>(OtherComp);
		//if (hasSBClicked)
		//{
		//	bIsOnVDP = false;
		//	DrawingPlane->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		//	DrawingPlane->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		//	DrawingPlane->SetWorldLocation(FVector(cursor->GetScaledSphereRadius(), 0.0f, 0.0f));
		//}

	}
}
void AHapticsHandler::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	//UE_LOG(LogTemp, Warning, TEXT("Finished"));
	////UE_LOG(LogTemp, Warning, TEXT("OverlappedComponent : %s"), *(OverlappedComp->GetName()));
	////UE_LOG(LogTemp, Warning, TEXT("OtherComp : %s"), *(OtherComp->GetName()));
	if (OtherComp->GetName() == "ProceduralPlaneMesh")
	{

		if (!hasFBClicked)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FString::Printf(TEXT("Finished")));
		}
	}
	/*
		if (bIntersected)
		{
			bIntersected = false;
			setForceToApply(FVector::ZeroVector);
			UE_LOG(LogTemp, Warning, TEXT("intersection finished"));
		}*/
	else if (OtherComp->GetName() == "DrawingPlane")
	{
		//bIsOnVDP = false;
	}
}
//
//void AHapticsHandler::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
//{
//	UE_LOG(LogTemp, Warning, TEXT("hitComponent : %s"), *(HitComponent->GetName()));
//
//}
//
//void AHapticsHandler::OnBeginOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
//{
//	UE_LOG(LogTemp, Warning, TEXT("OverlappedComp : %s"), *(OverlappedComp->GetName()));
//
//}



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
	return FVector((position.X * 1000) - 80.f, -position.Y * 1000, (position.Z * 1000));
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

	FRotator Rr = RootComponent->GetComponentRotation();
	FRotator r = FRotator(90.f, 0.f, 0.f);
	DrawingPlane->SetWorldLocation(brush->GetComponentLocation() + (this->GetActorForwardVector() + 0.05f));
	DrawingPlane->SetWorldRotation(Rr);
	DrawingPlane->AddLocalRotation(r);
	DrawingPlane->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	bIsOnVDP = true;

}

/*
	Redraw the cursor by the new brush information
*/

void AHapticsHandler::RefreshBrushCursor(float bSize, FLinearColor brushColor, float viscosity, FString tex)
{
	brush->ClearAllMeshSections();
	//UE_LOG(LogTemp, Warning, TEXT("Color: %s"), *(brushColor.ToString()));
	this->viscosity = viscosity;
	CreateBrushCursor(bSize, brushColor, tex);
	UE_LOG(LogTemp, Warning, TEXT("viscosity: %f"), viscosity);

}

void AHapticsHandler::CreateBrushCursor(float bSize, FLinearColor brushColor, FString tex)
{
	TArray<FVector> vertices;
	TArray<FVector> normals;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;
	TArray<FLinearColor> vertexColors;
	TArray<FProcMeshTangent> tangents;
	int32 height = 2;
	int32 width = 2;
	brushSize = bSize;
	float uvSpacing = 1.0f / FMath::Max(height, width);

	Material = LoadObject<UMaterialInterface>(nullptr, *tex);

	vertices.Add(FVector(0.0f, 1.0f, brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, 1.0f, -brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, brushSize / 2));
	vertexColors.Add(brushColor); //red

	vertices.Add(FVector(0.0f, -1.0f, -brushSize / 2));
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


