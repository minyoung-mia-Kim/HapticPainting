// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Haptico/Public/HapticsManager.h"
#include "Components/StaticMeshComponent.h"
#include "HapticHandler.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHapticCollisionDelegate, FVector, msPosition, FVector, msNormal, FVector, msTangent, FVector, cPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSbuttonDelegate, FVector, Position, bool, hasClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFbuttonDelegate, FVector, Position, bool, hasClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHapticModeDelegate);


class UStaticMeshComponent;

UCLASS(BlueprintType)
class HAPTICDRAWING_API AHapticsHandler : public AHapticsManager
{
	GENERATED_BODY()


		/*
		*	Mouse mode
		*/
		bool noHapticDevice;

public:
	UPROPERTY(EditDefaultsOnly, Category = "rc")
		USceneComponent* RComponent;

	UPROPERTY(VisibleAnywhere, Category = "Cursor")
		USphereComponent* cursor = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Cursor")
		UProceduralMeshComponent* brush = nullptr;

	FVector BrushNormal;
	UPROPERTY()
		float brushSize;
	UPROPERTY(EditAnywhere, Category = "MyProceduralMesh")
		UMaterialInterface* Material;
	UPROPERTY(VisibleAnywhere, Category = "VirtualDrawingPlane")
		UStaticMeshComponent* DrawingPlane = nullptr;


	/* Haptic status */
	bool hasFBClicked;
	bool hasSBClicked;
	bool bIsSpringOn;
	bool bIntersected;

	UPROPERTY()
		FVector FinalForce;


	/* Button DeltaTime & status check*/
	bool prvFBstat = false;
	float FButtonDt = 0.0f;
	float SButtonDt = 0.0f;

	/* Texture haptic force*/
	float viscosity = 5.0f;

	UFUNCTION()
		void setViscosity(float v);

	/* VDP Functions & Variables */
	FHitResult OutHit;
	bool bIsOnVDP = false;
	FVector bforce = FVector::ZeroVector;
	FVector VDPnormal;

	/* Adjust Haptic Handler */
	FRotator DDirection = FRotator::ZeroRotator;

	/* Activate Virtual Drawing Plane */
	UFUNCTION()
		void ActivateVDP();
public:

	AHapticsHandler();

	UPROPERTY()
		FHapticCollisionDelegate HapticCollisionData;
	UPROPERTY()
		FFbuttonDelegate FbuttonInputDelegate;
	UPROPERTY()
		FSbuttonDelegate SbuttonInputDelegate;
	UPROPERTY()
		FHapticModeDelegate FHapticModeUpdateDelegate;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type type) override;

public:

	virtual void Tick(float DeltaTime) override;

	/**
	* broad casts the new haptic data as a multicast delegate
	*/
	virtual void broadCastNewHapticData(FVector position, FMatrix rotation, FVector linearVelocity, FVector angularVelocity) override;

	/**
	* set the force that should be applied to the haptic device in the next tick of the haptic thread
	*/
	UFUNCTION(BlueprintCallable)
		static void setForceToApply(FVector force);

	/**
	* set the torque that should be applied to the haptic device in the next tick of the haptic thread
	*/
	UFUNCTION(BlueprintCallable)
		static void setTorqueToApply(FVector torque);

	/**
	* gets the current position of the haptic device end affector
	*/
	UFUNCTION(BlueprintCallable)
		FVector getHapticDevicePosition();

	/**
	* get the current position of the haptic device end affector in unreal coordinates
	*/
	UFUNCTION(BlueprintCallable)
		FVector getHapticDevicePositionInUnrealCoordinates();

	/**
	* get the current rotation of the haptic device end affector
	*/
	UFUNCTION(BlueprintCallable)
		FMatrix getHapticDeviceRotation();

	/**
	* get the current rotation of the haptic device end affector as an unreal rotator
	*/
	UFUNCTION(BlueprintCallable)
		FRotator getHapticDeviceRotationAsUnrealRotator();

	/**
	* get the current angular velocity of the haptic device end affector
	*/
	UFUNCTION(BlueprintCallable)
		FVector getHapticDeviceAngularVelocity();

	/**
	* get the current linear velocity of the haptic device end affector
	*/
	UFUNCTION(BlueprintCallable)
		FVector getHapticDeviceLinearVelocity();

	/**
	* fires an event when the button 1 is clicked
	*/
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Button1Clicked"))
	UFUNCTION(BlueprintCallable)
		void button1Clicked();

	/**
	* fires an event when the button 2 is clicked
	*/
	//UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Button2Clicked"))
	UFUNCTION(BlueprintCallable)
		void button2Clicked();

	/**
	* Collision event
	*/
	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
	//			UPrimitiveComponent* OtherComp, FVector NormalImpulse, 
	//			const FHitResult& Hit);
	//UFUNCTION()
	//	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	//		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	//		bool bFromSweep, const FHitResult& SweepResult);

public:
	/*
	* Redraw the brush cursor
	*/
	UFUNCTION()
		void RefreshBrushCursor(float bSize, FLinearColor brushColor, float viscosity, FString tex);

	UFUNCTION()
		void CreateBrushCursor(float bSize, FLinearColor brushColor, FString tex);
	UFUNCTION()
		void SetCursorRotation(FRotator rotation);
	

};
