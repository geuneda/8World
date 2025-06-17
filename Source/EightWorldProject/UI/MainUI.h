// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
public:
	
	UPROPERTY(meta=(BindWidget))
	class UMPUI* MPUI;

	UPROPERTY(meta=(BindWidget))
	class UImage* Crosshair;

	UPROPERTY(meta=(BindWidget))
	class UBuildModeUI* BuildModeUI;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txt_hp;
	
	UPROPERTY(BlueprintReadOnly, Category=HP)
	float hp = 1.f; //최대값
	
public:
	void SetCrosshair(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	void SetBuildModeUIVisible(bool bVisible);
	
};
