// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:  //---------UI Animation-------------
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* FadeOut;
};
