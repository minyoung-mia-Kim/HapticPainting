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
		FVector localized = GetActorTransform().InverseTransformPositionNoScale(CursorPosition);
		//UE_LOG(LogTemp, Warning, TEXT("mesh pos %s"), *(MeshPosition.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("localized plane pos %s"), *(GetActorTransform().GetLocation().ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("localized cursor pos %s"), *(localized.ToString()));


		// Calculate vector between b and a (giving the direction and magnitude of the vector they make up)
		CurrentForce = FVector(CursorPosition - MeshPosition);
		//UE_LOG(LogTemp, Warning, TEXT("basic force %s"), *(CurrentForce.ToString()));

		if (CurrentForce.Size() < SPRING_TOLERANCE)
		{
			CurrentForce = FVector::ZeroVector;
			return;
		}
		float dist = FVector::Dist(MeshPosition, CursorPosition); //has to calculate tick. Is it same with currentforce.size()?s
		CurrentForce.Normalize();
		float dotFN = FVector::DotProduct(CurrentForce, MeshNormal);
		float indotFN = FVector::DotProduct(CurrentForce, -MeshNormal);
		FVector proj = CurrentForce.ProjectOnToNormal(MeshNormal);
		//UE_LOG(LogTemp, Warning, TEXT("proj %s"), *(proj.ToString()));

		float intensity = dist * CurrentForce.Size();
		//CurrentForce = CurrentForce / CurrentForce.Size() * (intensity * dotFN * 0.1);
		//CurrentForce *= FVector(1.0f, 1.0f, 1.0f) - MeshTangent;
		if (localized.X > 0)
		{
			proj *= indotFN * 1.3;
			//CurrentForce = indotFN * CurrentForce;
			//CurrentForce *= -1;
		}
		else
		{
			proj *= dotFN;
			//CurrentForce = dotFN * CurrentForce;

		}
		CurrentForce = proj * springK;

		//UE_LOG(LogTemp, Warning, TEXT("Normal %s"), *(MeshNormal.ToString()));
		//UE_LOG(LogTemp, Warning, TEXT("force size %f"), CurrentForce.Size());
		//UE_LOG(LogTemp, Warning, TEXT("dist %f"), dist);
		//UE_LOG(LogTemp, Warning, TEXT("dot %f"), dotFN);
		//UE_LOG(LogTemp, Warning, TEXT("indot %f"), indotFN);
		//UE_LOG(LogTemp, Warning, TEXT("final force * dot%s"), *(CurrentForce.ToString()));
		DrawDebugLine(GetWorld(), MeshPosition, MeshPosition + CurrentForce * 10.f, FColor::Magenta, false, 0, 0, 0.5);
		HapticForceUpdate.Broadcast(CurrentForce);

	}

}

void AForceHandler::getForceInfo(FVector msPosition, FVector msNormal, FVector msTangent, FVector cPosition)
{
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
	Proxy->bVisible = false;
}

