// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoalWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UGoalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	//미션 체크 박스
	UPROPERTY(meta = (BindWidget))
	class UCheckBox* quest_checkBox;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_mission;

	//미션 체크 함수
	void OnCheckMissionCompleted(bool bIsCompleted);
};
