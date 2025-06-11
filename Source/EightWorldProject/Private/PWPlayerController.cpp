// Fill out your copyright notice in the Description page of Project Settings.


#include "PWPlayerController.h"
#include "GoalWidget.h"
#include "PWGameInstance.h"
#include "Components/CheckBox.h"

APWPlayerController::APWPlayerController()
{
	ConstructorHelpers::FClassFinder<UGoalWidget> tempGoalUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_GoalUI.WBP_GoalUI_C'"));
	if (tempGoalUI.Succeeded())
	{
		goalUIWidget = tempGoalUI.Class;
	}

	goalWidget = Cast<UGoalWidget>(goalUIWidget);
	
}
