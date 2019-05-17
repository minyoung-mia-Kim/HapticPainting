// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ColorPickerWidget.generated.h"

/**
 * 
 */
UCLASS()
class HAPTICDRAWING_API UColorPickerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UWidget* WidgetColorPicker = nullptr;
	
	
};
