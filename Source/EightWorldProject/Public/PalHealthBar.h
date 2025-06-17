// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PalHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UPalHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "HP")
	float hp = 1.f; //최대 체력
};
