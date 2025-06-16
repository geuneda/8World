// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "PWAIController.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
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

	
	//팰 Work 컴포넌트
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalWorkComponent* PalworkComp;

	//팰 에니메이션
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalYetiAnimInstance* YetiAnimInstance;
	
	//SetTableData TimerHandle
	FTimerHandle TableDataTimerHandle;
	//Work TimerHandle
	FTimerHandle WorkTimerHandle;

	//Escape TimerHandle
	FTimerHandle EscapeTimerHandle;

	//RagDoll TimerHandle
	//FTimerHandle RagDollTimerHandle;
	
	//팰 정찰 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PatrolRadius = 200.f;
	//플레이어 감지 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PlayerDetectDistance = 1000.f;
	//팰 공격 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AttackDistance = 300.f;
	
	//기준이 되는 위치
	UPROPERTY(visibleAnywhere)
	FVector InitLocation;
	
	//patrol 이동 여부
	UPROPERTY(ReplicatedUsing =  OnRep_Patrol)
	bool bIsPatroling = false;
	UFUNCTION()
	void OnRep_Patrol();

	//현재 타겟 위치
	FVector CurrentPatrolTargetLocation;

	//impulse
	FVector LaunchImpulse = FVector::ZeroVector;
	
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

	//팰 스피어 포획 당할때 호출되는 함수
	virtual void CaptureByPlayer() override;

	//팰이 도망가는 함수
	void UpdateEscapeLocation();

	//Impulse 관련 함수
	//void StopRagDollPhysics();
	
	//팰 모드 스위치 함수
	void SwitchWildState(float deltaTime);
	void SwitchBattleState();
	void SwitchWorkerState();
	
	//팰 Wild 모드 함수
	void HandleWildPatrol();
	void HandleWildPlayerHitToPal();
	void HandleWildDetectPlayer();
	void HandleWildChase();
	void HandleWildAttack(float deltaTime);
	void HandleWildEscape();
	void HandleWildDie();

	//팰 Battle 모드 함수
	void HandleBattleFollowPlayer();
	void HandleBattleDetectTarget();
	void HandleBattleChase();
	void HandleBattleAttack();
	void HandleBattleDie();

	//팰 Worker 모드 함수
	void HandleWorkerIdle();
	void HandleWorkerFindWork();
	void HandleWorkerMovetoTarget();
	void HandleWorkerWorking();
	void HandleWorkerReturn();

	//피격 데미지
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* IceBeamEffect;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* IceBeamEffect2;
	
public: //--------네트워크 RPC------------
	//채집 팰 WorkerIdle 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WorkerIdle(bool isPatrol);

	//채집 팰 FindWork 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WorkerFindWork(bool isPatrol);
	
	//채집 팰 WorkerMovetoTarget 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WorkerMovetoTarget(bool isMove, bool isWork);
	virtual void OnRep_MoveToTarget() override;
	virtual void OnRep_WorkAnim() override;

	//채집 팰 WorkerWorking 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WorkerWorking();

	//야생 팰 WildPlayerHitToPal 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildPlayerHitToPal();
	virtual void OnRep_Damaged() override;
	virtual void OnRep_AttackAnim() override;

	//야생 팰 WildChase 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildChasePatrol(); 
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildChaseMoveToTarget();

	//야생 팰 Escape 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildEscapeAttackAnim();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildEscapePatrol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildEscapeMoveToTarget();

	//야생 팰 Die 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildDie();
	
	
	//변수 동기화 함수
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};


