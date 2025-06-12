// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalWidget.h"

#include "PWGameInstance.h"
#include "PWPlayerController.h"
#include "Components/CheckBox.h"
#include "EightWorldProject/Player/PlayerCharacter.h"

void UGoalWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UGoalWidget::OnCheckMissionCompleted(bool bIsCompleted)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s][OnCheckMissionCompleted] bIsCompleted = %d"), GetWorld()->GetNetMode()==NM_Client?TEXT("Client") : TEXT("Server"), bIsCompleted);
	
	if (bIsCompleted)
	{
		// quest_checkBox->SetIsChecked(bIsCompleted);
		quest_checkBox->SetCheckedState(ECheckBoxState::Checked);
		UE_LOG(LogTemp, Warning, TEXT("[%s] [OnCheckMissionCompleted]"), GetWorld()->GetNetMode()==NM_Client?TEXT("Client") : TEXT("Server"));

	}
}

