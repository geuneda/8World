// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathWidget.h"

#include "PWPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

UDeathWidget::UDeathWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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

void UDeathWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_revival->OnHovered.AddDynamic(this, &UDeathWidget::UDeathWidget::OnHoveredButton);
	btn_revival->OnClicked.AddDynamic(this, &UDeathWidget::OnClickedButton);
}

void UDeathWidget::OnHoveredButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void UDeathWidget::OnClickedButton()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}

	this->SetVisibility(ESlateVisibility::Hidden);

	auto pc = Cast<APWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		// //입력 모드를 UI Only로 변경
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(this->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		pc->SetInputMode(InputMode);
	
		//마우스 커서 표시
		pc->bShowMouseCursor = false;
		//리스폰
		pc->ServerRPC_RespawnPlayer();
	}
}
