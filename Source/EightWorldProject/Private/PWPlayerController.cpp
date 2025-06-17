// Fill out your copyright notice in the Description page of Project Settings.


#include "PWPlayerController.h"
#include "GoalWidget.h"
#include "PWGameInstance.h"
#include "PWGameMode.h"
#include "Components/CheckBox.h"
#include "EightWorldProject/EightWorldProject.h"
#include "EightWorldProject/Player/PlayerCharacter.h"

APWPlayerController::APWPlayerController()
{
	
}

void APWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	myPlayer = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (HasAuthority())
	{
		gm = Cast<APWGameMode>(GetWorld()->GetAuthGameMode());

		if (!gm)
		{
			PRINTLOG(TEXT("GameMode is nullptr!"));
			return;
		}
	}
}

void APWPlayerController::ServerRPC_RespawnPlayer_Implementation()
{
	PRINTLOG(TEXT("RespawnPlayer"));
	//기존 pawn은 어떻게해??
	//->possess를 풀어야한다.
	auto player = GetPawn(); //플레이어 컨트롤러가 possess하고있는 것을 가져올 수 있다.
	UnPossess(); //포제스 풀기
	//->삭제해야한다.
	if (IsValid(player))
	{
		player->Destroy();
	}
	//새로운 pawn을 스폰하고 싶다.
	if (gm)
	{
		gm->RestartPlayer(this);
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APWPlayerController::RespawnAfterDelay, 0.1f, false);
	}

	
}

void APWPlayerController::RespawnAfterDelay()
{
	//gm->RestartPlayer(this);
}
