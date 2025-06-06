// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackDelegate);

/**
 * 플레이어 공격 컴포넌트
 * 공격 로직과 충돌 감지를 담당
 * 단일 공격 애니메이션과 AnimNotify 기반 타이밍 시스템 사용
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EIGHTWORLDPROJECT_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// 기본 값 설정
	UPlayerAttackComponent();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

public:	
	// 매 프레임 호출
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 공격 시작
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StartAttack();

	// 공격 중지
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StopAttack();

	// 공격 타이밍 알림 (AnimNotify에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void OnAttackTiming();

	// 공격 가능 여부 확인
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool CanAttack() const;

	// 공격 대상 감지
	UFUNCTION(BlueprintCallable, Category = "Attack")
	TArray<AActor*> DetectAttackTargets();

	// 공격 데미지 적용
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void ApplyDamageToTargets(const TArray<AActor*>& Targets);

public:
	// 공격 콜라이더
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	class UBoxComponent* AttackCollider;

	// 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDamage = 10.0f;

	// 공격 소모 기력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackStaminaCost = 10.0f;

	// 공격 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnAttackDelegate OnAttackExecuted;

private:
	// 플레이어 캐릭터 참조
	class APlayerCharacter* PlayerCharacter;
	
	// 플레이어 애니메이션 인스턴스 참조
	class UPlayerAnimInstance* PlayerAnimInstance;
	
	// 플레이어 스탯 컴포넌트 참조
	class UPlayerStatComp* PlayerStatComp;
	
	// 공격 중인지 여부
	bool bIsAttacking = false;
	
	// 공격 버튼을 계속 누르고 있는지 여부
	bool bIsAttackButtonPressed = false;
	
	// 공격 애니메이션 종료 후 다음 공격 시작
	void StartNextAttack();

public: //--------네트워크---------------------
	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyDamage(const TArray<AActor*>& targets);
};
