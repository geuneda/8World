// Fill out your copyright notice in the Description page of Project Settings.


#include "PWGameState.h"

#include "GoalWidget.h"
#include "MissionCompleteWidget.h"
#include "PWPlayerController.h"
#include "Components/TextBlock.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

APWGameState::APWGameState()
{
	SharedItemCount = 0;
}

void APWGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SharedItemCount = 0;
		OnRep_Count(); // UI도 초기화
	}
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
			pc->goalWidget = Cast<UGoalWidget>(player->GoalUI);
			pc->goalWidget->txt_mission->SetText(FText::Format(FText::FromString(TEXT("떨어진 목재나 돌맹이를 주워 10개를 획득하세요. ({0}/20)")),FText::AsNumber(SharedItemCount)));
			
			if (SharedItemCount >= 20)
			{
				pc->goalWidget->OnCheckMissionCompleted(true);

				if (player->missionCompleteUI)
				{
					pc->missionCompleteWidget = Cast<UMissionCompleteWidget>(player->missionCompleteUI);
					pc->missionCompleteWidget->AddToViewport();
					pc->SetShowMouseCursor(true);
					pc->missionCompleteWidget->PlayAnimation(pc->missionCompleteWidget->PopUp);

					if (pc->missionCompleteWidget->missionCompleteSound)
					{
						UGameplayStatics::PlaySound2D(this, pc->missionCompleteWidget->missionCompleteSound);
					}
				}
			}
			else
			{
				pc->goalWidget = Cast<UGoalWidget>(player->GoalUI);
				pc->goalWidget->OnCheckMissionCompleted(false);

				// if (player->missionCompleteUI)
				// {
				// 	pc->missionCompleteWidget = Cast<UMissionCompleteWidget>(player->missionCompleteUI);
				// 	pc->missionCompleteWidget->AddToViewport();
				// }
			}
		}


	}
}

void APWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APWGameState, SharedItemCount);
}
