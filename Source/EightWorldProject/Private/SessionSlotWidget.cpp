// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlotWidget.h"
#include "PWGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	btn_join->OnClicked.AddDynamic(this, &USessionSlotWidget::JoinSession);
}

void USessionSlotWidget::Set(const FSessionInfo& sessionInfo)
{
	txt_roomName->SetText(FText::FromString(sessionInfo.roomName));
	txt_hostName->SetText(FText::FromString(sessionInfo.hostName));
	txt_playerCount->SetText(FText::FromString(sessionInfo.playerCount));
	txt_pingSpeed->SetText(FText::FromString(FString::Printf(TEXT("%dms"), sessionInfo.pingSpeed)));

	sessionNumber = sessionInfo.index;

}

void USessionSlotWidget::JoinSession()
{
	auto gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->JoinSelectedSession(sessionNumber);
	}
}
