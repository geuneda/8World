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
		UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] 생성자에서 AttackMontage 로드 성공: %s"), *AttackMontage->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 생성자에서 AttackMontage 로드 실패"));
	}
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] NativeInitializeAnimation 호출"));
	
	// 소유 캐릭터 가져오기
	AActor* Owner = GetOwningActor();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 오류: GetOwningActor가 null"));
		return;
	}
	
	PlayerCharacter = Cast<APlayerCharacter>(Owner);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 오류: PlayerCharacter 캐스팅 실패"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] PlayerCharacter 설정 성공: %s"), *PlayerCharacter->GetName());
	}
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
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] PlayAttackMontage 호출"));
	
	// 공격 몽타주가 유효한지 확인
	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 오류: AttackMontage가 없음"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] AttackMontage 유효함: %s"), *AttackMontage->GetName());
	
	// 이미 재생 중인지 확인
	if (!IsAttacking())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] 몽타주 재생 시작"));
		Montage_Play(AttackMontage, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] 몽타주 재생 완료"));
		PlayerCharacter->PlayerStatComp->SetRestState(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] 이미 몽타주 재생 중"));
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

void UPlayerAnimInstance::AnimNotify_AttackTiming()
{
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] AnimNotify_AttackTiming 호출"));
	
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 오류: AnimNotify_AttackTiming에서 PlayerCharacter가 null"));
		return;
	}
	
	if (!PlayerCharacter->PlayerAttackComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimInstance] 오류: AnimNotify_AttackTiming에서 PlayerAttackComp가 null"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] OnAttackTiming 호출 시작"));
	PlayerCharacter->PlayerAttackComp->OnAttackTiming();
	UE_LOG(LogTemp, Warning, TEXT("[AnimInstance] OnAttackTiming 호출 완료"));
}
