// Fill out your copyright notice in the Description page of Project Settings.


#include "PWGameState.h"

#include "GoalWidget.h"
#include "MissionCompleteWidget.h"
#include "PWPlayerController.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

APWGameState::APWGameState()
{
	SharedItemCount = 0;
}

void APWGameState::SetSharedItemCount(int32 NewCount)
{
	if (HasAuthority())
	{
		SharedItemCount = NewCount;
		OnRep_Count(); //서버에서도 UI 갱신
	}
}

void APWGameState::OnRep_Count()
{
	auto pc = Cast<APWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		auto player = pc->GetPawn<APlayerCharacter>();

		if (player && player->GoalUI)
		{
			if (SharedItemCount >= 10)
			{
				pc->goalWidget = Cast<UGoalWidget>(player->GoalUI);
				pc->goalWidget->OnCheckMissionCompleted(true);

				if (player->missionCompleteUI)
				{
					pc->missionCompleteWidget = Cast<UMissionCompleteWidget>(player->missionCompleteUI);
					pc->missionCompleteWidget->AddToViewport();
				}
			}
		}


	}
}

void APWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APWGameState, SharedItemCount);
}
