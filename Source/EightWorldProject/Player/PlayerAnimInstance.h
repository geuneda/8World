// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 플레이어 애니메이션 인스턴스 클래스
 * 플레이어 캐릭터의 애니메이션 상태를 관리
 */
UCLASS()
class EIGHTWORLDPROJECT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 공격 애니메이션 재생
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAttackMontage();

	// 공격 애니메이션 중지
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void StopAttackMontage();

	// 공격 애니메이션 재생 중인지 확인
	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayPalSphereMontage();

	// 공격 타이밍 애님 노티파이
	UFUNCTION()
	void AnimNotify_AttackTiming();

	// 팰 스피어 투척 타이밍 애님 노티파이
	UFUNCTION()
	void AnimNotify_Throw();

	// 점프 애니메이션 재생
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayJumpMontage();
	

public:
	// 팰스피어 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* PalSphereMontage;
	
	// 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* AttackMontage;

	// 점프 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* JumpMontage;

	// Movement 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed;

	// 공중에 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsInAir;

	// 움직이는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsMoving;

private:
	// 소유 플레이어 캐릭터
	class APlayerCharacter* PlayerCharacter;

public: //--------------------네트워크--------------
};
