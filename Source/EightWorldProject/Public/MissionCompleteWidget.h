// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionCompleteWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UMissionCompleteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMissionCompleteWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_home;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* PopUp;

	//게임 종료 - 나가기
	UFUNCTION()
	void OnExitClicked();

public: //----------사운드---------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseHoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* missionCompleteSound;

	UFUNCTION()
	void OnHoveredHomeButton();
};
