// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildModeUI.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UBuildModeUI : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RockCostText;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TreeCostText;

	// 빌드 코스트 및 상태 표시 함수
	UFUNCTION(BlueprintCallable)
	void SetBuildCost(int32 TreeHave, int32 TreeNeed, int32 RockHave, int32 RockNeed, bool bCanBuild);
};
