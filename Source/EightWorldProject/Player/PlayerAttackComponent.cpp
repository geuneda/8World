// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerAttackComponent.h"
#include "PlayerCharacter.h"
#include "PlayerStatComp.h"
#include "PlayerAnimInstance.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

// 생성자
UPlayerAttackComponent::UPlayerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 공격 콜라이더 생성
	AttackCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollider"));
	
	// 콜라이더 설정
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollider->SetCollisionObjectType(ECC_Pawn);
	AttackCollider->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// 게임 시작 시 호출
void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("[Attack] BeginPlay 호출"));
	
	// 플레이어 캐릭터 참조 가져오기
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] 오류: PlayerCharacter 가져오기 실패"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Attack] PlayerCharacter 가져오기 성공: %s"), *PlayerCharacter->GetName());
	
	// 플레이어 스탯 컴포넌트 참조 가져오기
	PlayerStatComp = PlayerCharacter->PlayerStatComp;
	
	if (!PlayerStatComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] 오류: PlayerStatComp 가져오기 실패"));
	}
	
	// 플레이어 애니메이션 인스턴스 가져오기
	PlayerAnimInstance = Cast<UPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
	
	if (!PlayerAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] 오류: PlayerAnimInstance 가져오기 실패"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] PlayerAnimInstance 가져오기 성공"));
	}
	
	// 공격 콜라이더를 플레이어 캐릭터에 부착
	AttackCollider->AttachToComponent(PlayerCharacter->GetMesh(), 
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
		FName("AttackSocket")); // 소켓에 부착
	
	AttackCollider->SetRelativeLocation(FVector(0, 100, 0));
	UE_LOG(LogTemp, Warning, TEXT("[Attack] 공격 콜라이더 설정 완료"));
}

// 매 프레임 호출
void UPlayerAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// 공격 중이고 애니메이션이 종료되었는지 확인
	if (bIsAttacking && PlayerAnimInstance && !PlayerAnimInstance->IsAttacking())
	{
		// 공격 상태 해제
		bIsAttacking = false;
		
		// 콜라이더 비활성화
		AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// 공격 버튼을 계속 누르고 있다면 다음 공격 시작
		if (bIsAttackButtonPressed)
		{
			StartNextAttack();
		}
	}
}

// 공격 시작
void UPlayerAttackComponent::StartAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Attack] StartAttack 호출"));
	
	// 공격 버튼을 누르고 있는 상태로 설정
	bIsAttackButtonPressed = true;
	
	// 공격 중이면 다음 공격을 위해 상태만 설정하고 리턴
	if (bIsAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 이미 공격 중, 다음 공격 준비"));
		return;
	}
	
	// 공격 가능 여부 확인
	if (!CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 공격 불가: CanAttack() 검사 실패"));
		return;
	}
	
	// 공격 상태 설정
	bIsAttacking = true;
	UE_LOG(LogTemp, Warning, TEXT("[Attack] 공격 상태 설정 완료"));
	
	// 기력 소모
	if (PlayerStatComp)
	{
		PlayerStatComp->MP -= AttackStaminaCost;
		PlayerStatComp->MP = FMath::Clamp(PlayerStatComp->MP, 0.0f, PlayerStatComp->MaxMP);
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 기력 소모 완료: %f"), PlayerStatComp->MP);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] 오류: PlayerStatComp가 없음"));
	}
	
	// 애니메이션 인스턴스를 통해 공격 몽타주 재생
	if (PlayerAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 애니메이션 인스턴스 호출 시작"));
		PlayerAnimInstance->PlayAttackMontage();
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 애니메이션 인스턴스 호출 완료"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Attack] 오류: PlayerAnimInstance가 없음"));
	}
}

// 공격 중지
void UPlayerAttackComponent::StopAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Attack] StopAttack 호출"));
	
	// 공격 버튼 해제 상태로 설정
	bIsAttackButtonPressed = false;

	PlayerAnimInstance->StopAttackMontage();
}

// 공격 타이밍 알림 (AnimNotify에서 호출)
void UPlayerAttackComponent::OnAttackTiming()
{
	// 공격 중이 아니면 무시
	if (!bIsAttacking)
	{
		return;
	}
	
	// 콜라이더 활성화
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	// 공격 대상 감지
	TArray<AActor*> Targets = DetectAttackTargets();
	
	// 감지된 대상에 데미지 적용
	ApplyDamageToTargets(Targets);
	
	// 공격 이벤트 발생
	OnAttackExecuted.Broadcast();
	
	// 콜라이더 비활성화 (타이밍에만 잡아야 함)
	AttackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 공격 가능 여부 확인
bool UPlayerAttackComponent::CanAttack() const
{
	// 플레이어 스탯 컴포넌트 확인
	if (!PlayerStatComp)
	{
		return false;
	}
	
	// 기력 확인
	if (PlayerStatComp->MP < AttackStaminaCost)
	{
		return false;
	}
	
	return true;
}

// 공격 애니메이션 종료 후 다음 공격 시작
void UPlayerAttackComponent::StartNextAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("[Attack] StartNextAttack 호출"));
	
	// 공격 가능 여부 확인
	if (!CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 다음 공격 불가: CanAttack() 검사 실패"));
		return;
	}
	
	// 공격 상태 설정
	bIsAttacking = true;
	UE_LOG(LogTemp, Warning, TEXT("[Attack] 다음 공격 상태 설정 완료"));
	
	// 기력 소모
	if (PlayerStatComp)
	{
		PlayerStatComp->MP -= AttackStaminaCost;
		PlayerStatComp->MP = FMath::Clamp(PlayerStatComp->MP, 0.0f, PlayerStatComp->MaxMP);
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 다음 공격 기력 소모 완료: %f"), PlayerStatComp->MP);
	}
	
	// 애니메이션 인스턴스를 통해 공격 몽타주 재생
	if (PlayerAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 다음 공격 애니메이션 재생 시작"));
		PlayerAnimInstance->PlayAttackMontage();
		UE_LOG(LogTemp, Warning, TEXT("[Attack] 다음 공격 애니메이션 재생 완료"));
	}
}

// 공격 대상 감지
TArray<AActor*> UPlayerAttackComponent::DetectAttackTargets()
{
	TArray<AActor*> OverlappingActors;
	
	// 콜라이더와 겹치는 액터 가져오기
	AttackCollider->GetOverlappingActors(OverlappingActors);
	
	// 자기 자신 제외
	TArray<AActor*> ValidTargets;
	for (AActor* Actor : OverlappingActors)
	{
		// 자기 자신 제외
		if (Actor == GetOwner())
		{
			continue;
		}
		
		// 데미지를 받을 수 있는 대상만 추가
		if (Actor->CanBeDamaged())
		{
			ValidTargets.Add(Actor);
		}
	}
	
	return ValidTargets;
}

// 공격 데미지 적용
void UPlayerAttackComponent::ApplyDamageToTargets(const TArray<AActor*>& Targets)
{
	// 데미지 적용
	for (AActor* Target : Targets)
	{
		if (Target)
		{
			UGameplayStatics::ApplyDamage(
				Target,                                     // 대상
				AttackDamage,                               // 데미지 양
				PlayerCharacter->GetController(),           // 데미지 원인 컨트롤러
				PlayerCharacter,                            // 데미지 원인 액터
				UDamageType::StaticClass()                  // 데미지 유형
			);
		}
	}
}
