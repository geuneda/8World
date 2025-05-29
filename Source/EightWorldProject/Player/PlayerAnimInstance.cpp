// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAnimInstance.h"

#include "PlayerAttackComponent.h"
#include "PlayerCharacter.h"
#include "PlayerStatComp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	// 변수 초기화
	Speed = 0.0f;
	bIsInAir = false;
	bIsMoving = false;
	PlayerCharacter = nullptr;
	
	// 공격 몽타주 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageObj(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Player/AM_Attack.AM_Attack'"));
	if (AttackMontageObj.Succeeded())
	{
		AttackMontage = AttackMontageObj.Object;
	}

	// 점프 몽타주 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> JumpMontageObj(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Player/AM_Jump.AM_Jump'"));
	if (JumpMontageObj.Succeeded())
	{
		JumpMontage = JumpMontageObj.Object;
	}
	
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// 소유 캐릭터 가져오기
	AActor* Owner = GetOwningActor();
	if (!Owner)
	{
		return;
	}
	
	PlayerCharacter = Cast<APlayerCharacter>(Owner);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// 소유 캐릭터가 유효한지 확인
	if (!PlayerCharacter)
	{
		return;
	}
	
	// 캐릭터 이동 컴포넌트 가져오기
	UCharacterMovementComponent* MovementComponent = PlayerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}
	
	// 속도 업데이트
	FVector Velocity = PlayerCharacter->GetVelocity();
	Speed = Velocity.Size();
	
	// 공중에 있는지 여부 업데이트
	bIsInAir = MovementComponent->IsFalling();
	
	// 움직이는지 여부 업데이트
	bIsMoving = Speed > 3.0f;
}

// 공격 애니메이션 재생
void UPlayerAnimInstance::PlayAttackMontage()
{
	// 공격 몽타주가 유효한지 확인
	if (!AttackMontage)
	{
		return;
	}
	
	// 이미 재생 중인지 확인
	if (!IsAttacking())
	{
		Montage_Play(AttackMontage, 1.0f);
		PlayerCharacter->PlayerStatComp->SetRestState(false);
	}
}

// 공격 애니메이션 중지
void UPlayerAnimInstance::StopAttackMontage()
{
	// 공격 몽타주가 유효한지 확인
	if (!AttackMontage)
	{
		return;
	}
	
	// 현재 재생 중인 몽타주 중지
	Montage_Stop(0.25f, AttackMontage);
	PlayerCharacter->PlayerStatComp->SetRestState(true);
}

// 공격 애니메이션 재생 중인지 확인
bool UPlayerAnimInstance::IsAttacking() const
{
	// 현재 몽타주가 재생 중인지 확인
	return Montage_IsPlaying(AttackMontage);
}

void UPlayerAnimInstance::PlayJumpMontage()
{
	// 공격 몽타주가 유효한지 확인
	if (!JumpMontage)
	{
		return;
	}

	bIsInAir = true;
	
	Montage_Play(JumpMontage, 1.0f);
}

void UPlayerAnimInstance::AnimNotify_AttackTiming()
{
	if (!PlayerCharacter)
	{
		return;
	}
	
	if (!PlayerCharacter->PlayerAttackComp)
	{
		return;
	}
	PlayerCharacter->PlayerAttackComp->OnAttackTiming();
}
