// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ESCWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UESCWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UESCWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_backTitle;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_noBackTitle;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* PopUp;

public: //-------------사운드--------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseHoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* escSound;

	UFUNCTION()
	void OnHoveredButton();
	UFUNCTION()
	void OnClickedYESButton();
	UFUNCTION()
	void OnClickedNOButton();
};
