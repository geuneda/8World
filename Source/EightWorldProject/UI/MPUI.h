// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPUI.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UMPUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UMPUI(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// MP 프로그레스 바
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* MPProgressBar;

	// MP 설정 함수
	UFUNCTION()
	void SetMP(float mP, float maxMP);

private:
	// 현재 표시 비율
	float CurrentPercent;

	// 목표 비율
	float TargetPercent;

	// 전환 속도
	UPROPERTY(EditAnywhere, Category = "Animation")
	float LerpSpeed = 5.0f;
};
