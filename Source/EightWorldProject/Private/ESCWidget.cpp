// Fill out your copyright notice in the Description page of Project Settings.


#include "ESCWidget.h"

#include "PWGameInstance.h"
#include "Components/Button.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

UESCWidget::UESCWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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

void UESCWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_backTitle->OnHovered.AddDynamic(this, &UESCWidget::OnHoveredButton);
	btn_backTitle->OnClicked.AddDynamic(this, &UESCWidget::OnClickedYESButton);
	
	btn_noBackTitle->OnHovered.AddDynamic(this, &UESCWidget::OnHoveredButton);
	btn_noBackTitle->OnClicked.AddDynamic(this, &UESCWidget::OnClickedNOButton);
}

void UESCWidget::OnHoveredButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void UESCWidget::OnClickedYESButton()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}

	auto gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->ExitRoom();
	}
}

void UESCWidget::OnClickedNOButton()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}

	auto pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!pc) return;
	
	if (this)
	{
		this->SetVisibility(ESlateVisibility::Hidden);
	}


	// //입력 모드를 UI Only로 변경
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	pc->SetInputMode(InputMode);
	
	//마우스 커서 표시
	pc->bShowMouseCursor = false;

}
