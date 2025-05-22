// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "GameFramework/Character.h"
#include "PalAlpaca.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APalAlpaca : public APal
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APalAlpaca();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//팰 메시
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalWorkComponent* PalworkComp;

	//팰 모드 스위치 함수
	void SwitchWildState();
	void SwitchBattleState();
	void SwitchWorkerState();
	
	//팰 Wild 모드 함수
	void HandleWildPatrol();
	void HandleWildPlayerHitToPal();
	void HandleWildDetectPlayer();
	void HandleWildChase();
	void HandleWildAttack();
	void HandleWildReturn();

	//팰 Battle 모드 함수
	void HandleBattleFollowPlayer();
	void HandleBattleDetectTarget();
	void HandleBattleChase();
	void HandleBattleAttack();

	//팰 Worker 모드 함수
	void HandleWorkerIdle();
	void HandleWorkerFindWork();
	void HandleWorkerMovetoTarget();
	void HandleWorkerWorking();
	void HandleWorkerReturn();
	
};
