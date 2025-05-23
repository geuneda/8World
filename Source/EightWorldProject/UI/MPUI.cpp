// Fill out your copyright notice in the Description page of Project Settings.


#include "MPUI.h"
#include "Components/ProgressBar.h"


UMPUI::UMPUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CurrentPercent(1.0f)
	, TargetPercent(1.0f)
{
}

// 위젯 초기화 함수
void UMPUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 초기 색상 설정
	if (MPProgressBar)
	{
		MPProgressBar->SetPercent(1.0f);
		MPProgressBar->SetFillColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // 초록색으로 시작
	}
}

void UMPUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// 현재 퍼센트를 목표 퍼센트로 부드럽게 전환
	if (MPProgressBar && !FMath::IsNearlyEqual(CurrentPercent, TargetPercent, 0.001f))
	{
		// Lerp를 사용하여 부드럽게 전환
		CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, LerpSpeed);
		MPProgressBar->SetPercent(CurrentPercent);
		
		// 색상 계산 및 적용
		FLinearColor BarColor;
		
		if (CurrentPercent > 0.5f)
		{
			// 녹색에서 노란색으로 전환 (1.0에서 0.5)
			float RedComponent = FMath::Lerp(0.0f, 1.0f, (1.0f - CurrentPercent) * 2.0f);
			BarColor = FLinearColor(RedComponent, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			// 노란색에서 빨간색으로 전환 (0.5에서 0.0)
			float GreenComponent = FMath::Lerp(0.0f, 1.0f, CurrentPercent * 2.0f);
			BarColor = FLinearColor(1.0f, GreenComponent, 0.0f, 1.0f);
		}
		
		MPProgressBar->SetFillColorAndOpacity(BarColor);
	}
}

// MP 설정 함수
void UMPUI::SetMP(float mP, float maxMP)
{
	if (MPProgressBar)
	{
		// 목표 퍼센트 계산 (0.0 ~ 1.0 사이로 제한)
		TargetPercent = FMath::Clamp(mP / maxMP, 0.0f, 1.0f);
		
		// NativeTick에서 부드러운 전환 처리
	}
}
