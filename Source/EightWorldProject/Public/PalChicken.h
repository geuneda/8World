// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "PalChicken.generated.h"


UCLASS()
class EIGHTWORLDPROJECT_API APalChicken : public APal
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APalChicken();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//팰 에니메이션
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalChickenAnimInstance* ChickenAnimInstance;

	//팰 정찰 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PatrolRadius = 200.f;
	//플레이어 감지 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PlayerDetectRadius = 300.f;
	
	//기준이 되는 위치
	UPROPERTY(visibleAnywhere)
	FVector InitLocation;

	//Patrol 반복 타이머
	FTimerHandle PatrolTimerHandle;

	//Escape TimerHandle
	FTimerHandle EscapeTimerHandle;

	//patrol 이동 여부
	UPROPERTY(ReplicatedUsing =  OnRep_Patrol)
	bool bIsPatroling = false;
	UFUNCTION()
	void OnRep_Patrol();

	//현재 타겟 위치
	FVector CurrentPatrolTargetLocation;

	//SetTableData TimerHandle
	FTimerHandle TableDataTimerHandle;

	//팰이 충돌 체크를 위한 Sphere Collider
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class USphereComponent* SphereComp;

	//AIController
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class APWAIController* MyAIController;

	//플레이어 감지 범위
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float PlayerDetectDistance = 1500.f;

	//impulse
	FVector LaunchImpulse = FVector::ZeroVector;
	
	//팰 데이터 받아오는 함수
	virtual void SetTableData() override;

	//팰 운반중 get set
	virtual bool GetPalIsCarrying() const override;
	virtual void SetPalIsCarrying(bool IsCarrying) override;

	//팰 스피어 포획 당할때 호출되는 함수
	virtual void CaptureByPlayer() override;
	
	//타이머용 함수, 아이템의 최종위치 이용하기 위한 함수
	void SetCarrierMoveToTarget();

	//타이머용 함수, 아이템 일정 시간 뒤에 삭제 함수
	void CarriedItemDestroy();

	//팰이 도망가는 함수
	void UpdateEscapeLocation();
		
	//팰 모드 Set함수
	virtual void SetPalMode(EPalMode Mode) override;

	//팰 모드별 상태 Set함수
	virtual void SetPalWildState(EPalWildState State) override;
	virtual void SetPalBattleState(EPalBattleState State) override;
	virtual void SetPalCarrierState(EPalCarrierState State, AActor* TargetActor) override;
	
	//팰 모드 스위치 함수
	void SwitchWildState();
	void SwitchBattleState();
	void SwitchCarrierState();
	
	//팰 Wild 모드 함수
	void HandleWildPatrol();
	void HandleWildPlayerHitToPal();
	void HandleWildDetectPlayer();
	void HandleWildChase();
	void HandleWildAttack();
	void HandleWildEscape();
	void HandleWildDie();

	//팰 Battle 모드 함수
	void HandleBattleFollowPlayer();
	void HandleBattleDetectTarget();
	void HandleBattleChase();
	void HandleBattleAttack();
	void HandleBattleDie();

	//팰 Carrier 모드 함수
	void HandleCarrierPatrol();
	void HandleCarrierFindItem();
	void HandleCarrierMovetoTarget();
	void HandleCarrierCarrying();
	void HandleCarrierReturn();

	//피격 데미지
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public: //--------네트워크 RPC------------
	//채집 팰 CarrierPatrol 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_CarrierPatrol(bool isPatrol);

	//채집 팰 FindItem 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_CarrierFindItem(bool isPatrol);

	//채집 팰 CarrierMovetoTarget 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_CarrierMovetoTarget(bool isMove); 
	virtual void OnRep_MoveToTarget() override;

	 //채집 팰 CarrierCarrying 함수
	 UFUNCTION(NetMulticast, Reliable)
	 void MultiRPC_CarrierCarrying(AResourceItem* item); 
	
	//야생 팰 WildPlayerHitToPal 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildPlayerHitToPal();
	virtual void OnRep_Damaged() override;
	virtual void OnRep_AttackAnim() override;

	//야생 팰 WildPatrol 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildPatrol(); 
	//야생 팰 WildMoveToTarget 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildMoveToTarget();

	//야생 팰 Die 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_WildDie();
	
	//변수 동기화 함수
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};

