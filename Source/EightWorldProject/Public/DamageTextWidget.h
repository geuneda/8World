// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_damage;

	UFUNCTION(BlueprintCallable)
	void SetDamageText(int32 DamageAmount);

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeAndMove;
};
