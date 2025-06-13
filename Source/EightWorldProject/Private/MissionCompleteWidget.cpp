// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionCompleteWidget.h"

#include "PWGameInstance.h"
#include "Components/Button.h"

void UMissionCompleteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_home->OnClicked.AddDynamic(this, &UMissionCompleteWidget::OnExitClicked);
}

void UMissionCompleteWidget::OnExitClicked()
{
	auto gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->ExitRoom();
	}
}
