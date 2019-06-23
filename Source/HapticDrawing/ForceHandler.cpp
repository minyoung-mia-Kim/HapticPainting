// Fill out your copyright notice in the Description page of Project Settings.

#include "ForceHandler.h"
#include "DrawDebugHelpers.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"



// Sets default values
AForceHandler::AForceHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Proxy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("proxy"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshToUse(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	Proxy->SetStaticMesh(MeshToUse.Object);
	//Proxy->SetWorldLocation(FVector::ZeroVector);
	Proxy->SetRelativeScale3D(FVector(0.01f, 0.01f, 0.01f));
	Proxy->SetupAttachment(RootComponent);
	Proxy->bVisible = false;


	MeshPosition = FVector::ZeroVector;
	MeshNormal = FVector::ZeroVector;
	MeshTangent = FVector::ZeroVector;
	CursorPosition = FVector::ZeroVector;
	CurrentForce = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AForceHandler::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AForceHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* Calculate Force 
	   Reference : http://www.ryanmwright.com/tag/c/
	*/
	if (MeshPosition != FVector::ZeroVector)
	{
		Proxy->bVisible = true;
		SetActorLocation(MeshPosition);
		//FVector localized = GetActorTransform().InverseTransformPositionNoScale(CursorPosition);
		//UE_LOG(LogTemp, Warning, TEXT("mesh pos %s"), *(MeshPosition.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("localized plane pos %s"), *(GetActorTransform().GetLocation().ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("localized cursor pos %s"), *(localized.ToString()));
		UE_LOG(LogTemp, Warning, TEXT("MeshNormal %s"), *(MeshNormal.ToString()));


		// Calculate vector between b and a (giving the direction and magnitude of the vector they make up)
		FVector distance = FVector(CursorPosition - MeshPosition);
		float distDotNorm = FVector::DotProduct(distance, MeshNormal); // |dist||Mn|cos(theta)
		FVector desirePos = CursorPosition - (distDotNorm / MeshNormal.Size()) * MeshNormal;
		FVector proj = (distDotNorm * MeshNormal) / FMath::Pow(MeshNormal.Size(),2);

		//DrawDebugPoint(GetWorld(), desirePos, 10.f, FColor::Yellow, false, 5.f, 0.0f);
		float pf = proj.Size();
		//UE_LOG(LogTemp, Warning, TEXT("desirePos %s"), *(desirePos.ToString()));
		proj.Normalize();
		//UE_LOG(LogTemp, Warning, TEXT("distance %s"), *(distance.ToString()));
		float f = ClampForce(pf);
		//UE_LOG(LogTemp, Warning, TEXT("force size %f"), pf/10);
		
		CurrentForce = -proj;
		UE_LOG(LogTemp, Warning, TEXT("proj %s"), *(proj.ToString()));
		FVector dPos = FVector(CursorPosition - desirePos);
		dPos.Normalize();
		UE_LOG(LogTemp, Warning, TEXT("CursorPosition - Desire %s"), *(dPos.ToString()));

		//CurrentForce.Normalize();
		//CurrentForce += proj * springK;

		if (CurrentForce.Size() < SPRING_TOLERANCE)
		{
			CurrentForce = FVector::ZeroVector;
			return;
		}
		//else if (CurrentForce.Size() > ForceClamp)
		//{
		//	CurrentForce.Normalize();
		//	CurrentForce *= ForceClamp;
		//}

		FVector localProj = GetActorTransform().TransformFVector4NoScale(proj);
		localProj.Normalize();

		//DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + (localProj*pf), FColor::Magenta, false, 0, 0, 0.5);
		//DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + (proj*pf), FColor::Cyan, false, 0, 0, 0.5);
		//DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + (MeshNormal*pf), FColor::Red, false, 0, 0, 0.5);
		//DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + (dPos*pf), FColor::Green, false, 0, 0, 0.5);



		//FVector localPos = GetActorTransform().TransformPositionNoScale(MeshPosition);

		UE_LOG(LogTemp, Warning, TEXT("localProj %s"), *(localProj.ToString()));
		FVector lMp = localProj * proj;
		//DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + (-lMp * pf), FColor::Black, false, 0, 0, 0.5);

		//lMp.Y = -lMp.Y;
		FVector force = HHandlerRotator.RotateVector(FVector(-dPos.X, dPos.Y, dPos.Z));
		HapticForceUpdate.Broadcast(force*(-1.0f * pf/5));
		//HapticForceUpdate.Broadcast(FVector(0.0f, 1.0f, 0.0f));

		//UE_LOG(LogTemp, Warning, TEXT("localProj * Prog %s"), *(lMp.ToString()));



		//float dist = FVector::Dist(MeshPosition, CursorPosition); //has to calculate tick. Is it same with currentforce.size()?s
		////CurrentForce.Normalize();

		//float dotFN = FVector::DotProduct(CurrentForce, MeshNormal);
		//float indotFN = FVector::DotProduct(CurrentForce, -MeshNormal);


		////UE_LOG(LogTemp, Warning, TEXT("proj %s"), *(proj.ToString()));

		//float intensity = dist * CurrentForce.Size();
		//CurrentForce = CurrentForce / CurrentForce.Size() * (intensity * dotFN * 0.1);
		//CurrentForce *= FVector(1.0f, 1.0f, 1.0f) - MeshTangent;
		//if (localized.X > 0)
		//{
		//	proj *= indotFN * 1.3;
		//	//CurrentForce = indotFN * CurrentForce;
		//	//CurrentForce *= -1;
		//}
		//else
		//{
		//	proj *= dotFN;
		//	//CurrentForce = dotFN * CurrentForce;

		//}
		////CurrentForce = proj * springK;

		//UE_LOG(LogTemp, Warning, TEXT("force size %f"), CurrentForce.Size());
		//UE_LOG(LogTemp, Warning, TEXT("dist %f"), dist);
		//UE_LOG(LogTemp, Warning, TEXT("dot %f"), dotFN);
		//UE_LOG(LogTemp, Warning, TEXT("indot %f"), indotFN);
		//UE_LOG(LogTemp, Warning, TEXT("final force * dot%s"), *(CurrentForce.ToString()));

	}

}
float AForceHandler::ClampForce(float force)
{
	/*float _f = FMath::Abs(ForceClamp - force);
	if (_f < 0.0f)
	{
		return 0.0f;
	}
	else if (_f > ForceClamp)
	{
		return ForceClamp;
	}
	else
	{
		return _f;
	}*/
	return 0;

}
void AForceHandler::getForceInfo(FVector msPosition, FVector msNormal, FVector msTangent, FVector cPosition)
{
	UE_LOG(LogTemp, Warning, TEXT("get force info"));
	//UE_LOG(LogTemp, Warning, TEXT("centmsPositionorPos %s"), *msPosition.ToString());
	MeshPosition = msPosition;
	MeshNormal = msNormal;
	MeshTangent = msTangent;
	//CursorPosition = cPosition;
}

void AForceHandler::cleanForceInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("clean force info"));
	MeshPosition = FVector::ZeroVector;
	MeshNormal = FVector::ZeroVector;
	MeshTangent = FVector::ZeroVector;
	CursorPosition = FVector::ZeroVector;
	CurrentForce = FVector::ZeroVector;
	HapticForceUpdate.Broadcast(CurrentForce);
	Proxy->bVisible = false;
}

