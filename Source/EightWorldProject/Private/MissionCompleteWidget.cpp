// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionCompleteWidget.h"

#include "PWGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

UMissionCompleteWidget::UMissionCompleteWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
	
	ConstructorHelpers::FObjectFinder<USoundBase> tempcompleteSound(TEXT("/Game/PalWorld/Sound/missionComplete"));
	if (tempcompleteSound.Succeeded())
	{
		missionCompleteSound = tempcompleteSound.Object;
	}
}

void UMissionCompleteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_home->OnClicked.AddDynamic(this, &UMissionCompleteWidget::OnExitClicked);
	btn_home->OnHovered.AddDynamic(this, &UMissionCompleteWidget::OnHoveredHomeButton);
}

void UMissionCompleteWidget::OnExitClicked()
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

void UMissionCompleteWidget::OnHoveredHomeButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}
