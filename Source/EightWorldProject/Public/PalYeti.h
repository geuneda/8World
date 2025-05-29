// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "GameFramework/Character.h"
#include "PalYeti.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APalYeti : public APal
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APalYeti();

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

	//팰 에니메이션
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalYetiAnimInstance* YetiAnimInstance;
	
	//SetTableData TimerHandle
	FTimerHandle TableDataTimerHandle;
	//Work TimerHandle
	FTimerHandle WorkTimerHandle; 
	
	//팰 데이터 받아오는 함수
	virtual void SetTableData() override;

	//팰 작업 실행 함수
	virtual void PalWorking() override;
	
	//팰 모드 Set함수
	virtual void SetPalMode(EPalMode Mode) override;

	//팰 모드별 상태 Set함수
	virtual void SetPalWildState(EPalWildState State) override;
	virtual void SetPalBattleState(EPalBattleState State) override;
	virtual void SetPalWorkerState(EPalWorkerState State, AActor* TargetActor) override;

	//팰 작업중 get set
	UFUNCTION(BlueprintCallable, Category = "Pal")
	virtual bool GetPalIsWorking() const override;
	virtual void SetPalIsWorking(bool IsWorking) override;
	
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
	void HandleWildEscape();
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
