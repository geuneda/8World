// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlotWidget.h"

#include "LoadingWidget.h"
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

	//EnterIngame Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempEnterSound(TEXT("/Game/PalWorld/Sound/_EnterIngame_665203__silverillusionist__level-upmission-complete-fantasy"));
	if (tempEnterSound.Succeeded())
	{
		enterIngameSound = tempEnterSound.Object;
	}
		
	//loading widget
	ConstructorHelpers::FClassFinder<ULoadingWidget> tempLoadingWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_Loading.WBP_Loading_C'"));
	if (tempLoadingWidget.Succeeded())
	{
		loadingWidgetClass = tempLoadingWidget.Class;
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

	if (loadingWidgetClass)
	{
		loadingWidget = CreateWidget<ULoadingWidget>(GetWorld(), loadingWidgetClass);
		if (loadingWidget)
		{
			loadingWidget->AddToViewport();
			loadingWidget->PlayAnimation(loadingWidget->FadeIn);
			
			const float TotalDelay = 2.0f;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				if (enterIngameSound)
				{
					UGameplayStatics::PlaySound2D(this, enterIngameSound);
				}
				
				FTimerHandle ExitHandle;
				GetWorld()->GetTimerManager().SetTimer(ExitHandle, [&]()
				{
					//값이 있을때 입장하기
					auto gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
					if (gi)
					{
						gi->JoinSelectedSession(sessionNumber);
						//loadingWidget->PlayAnimation(loadingWidget->FadeOut);
					}
				}, 1.5f, false);
				
			}, TotalDelay, false);
		}
	}
	

	

}

void USessionSlotWidget::OnHoveredJoinButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}
