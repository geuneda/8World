// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlotWidget.h"
#include "PWGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

USessionSlotWidget::USessionSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Hover Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempHoverSound(TEXT("/Game/PalWorld/Sound/_ButtonHover_422836__gamedevc__g_ui_button_hover_1"));
	if (tempHoverSound.Succeeded())
	{
		mouseHoverSound = tempHoverSound.Object;
	}

	//Click Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempClickSound(TEXT("/Game/PalWorld/Sound/_ButtonClick_digital-click-357350"));
	if (tempClickSound.Succeeded())
	{
		mouseClickSound = tempClickSound.Object;
	}
}

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	btn_join->OnClicked.AddDynamic(this, &USessionSlotWidget::JoinSession);
	btn_join->OnHovered.AddDynamic(this, &USessionSlotWidget::OnHoveredJoinButton);
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
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	
	auto gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->JoinSelectedSession(sessionNumber);
	}
}

void USessionSlotWidget::OnHoveredJoinButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}
