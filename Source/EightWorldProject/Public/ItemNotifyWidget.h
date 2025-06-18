// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNotifyWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UItemNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* itemicon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_itemName;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* UPAnim;

	//세팅하기
	void Setup(FText Name, UTexture2D* Icon);
	
};
