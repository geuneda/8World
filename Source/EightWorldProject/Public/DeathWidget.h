// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UDeathWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDeathWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_revival;

public: //-------------사운드--------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseHoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseClickSound;

	UFUNCTION()
	void OnHoveredButton();
	UFUNCTION()
	void OnClickedButton();
};
