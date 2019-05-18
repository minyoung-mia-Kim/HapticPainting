// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "ColorPicker.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FColorUpdateDelegate, FLinearColor, selectedColor);

UCLASS()
class HAPTICDRAWING_API AColorPicker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorPicker();


	/* Color picker components */
	UPROPERTY(VisibleAnywhere, Category = "Colorpicker")
		USceneComponent* Root = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Colorpicker")
		UBoxComponent* ColorPickerCanvus = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Colorpicker")
		UProceduralMeshComponent* ColorPalette = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Colorpicker")
		FLinearColor Hue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Colorpicker")
		UMaterialInstanceDynamic* DynamicMatInstance;

	/* Function Picker */
	UFUNCTION()
		void CreateColorPalette();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
