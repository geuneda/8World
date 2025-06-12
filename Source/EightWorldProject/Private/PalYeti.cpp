// Fill out your copyright notice in the Description page of Project Settings.


#include "PalYeti.h"

#include "NavigationSystem.h"
#include "PalYetiAnimInstance.h"
#include "PalBox.h"
#include "PalWorkComponent.h"
#include "PWAIController.h"
#include "Components/CapsuleComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/Resources/Rock.h"
#include "EightWorldProject/Resources/Tree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DECLARE_LOG_CATEGORY_EXTERN(PalYeti, Log, All);
DEFINE_LOG_CATEGORY(PalYeti);

// Sets default values
APalYeti::APalYeti()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Yeti Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Pal/Yeti/Yeti.Yeti'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90.f), FRotator(0,-90,0));
		GetMesh()->SetIsReplicated(true);
		SetReplicates(true);
		SetReplicateMovement(true);
	}
	
	PalworkComp = CreateDefaultSubobject<UPalWorkComponent>(TEXT("PalWorkComp"));

	//DataTable RowName 초기화
	PalDataRowName = TEXT("Yeti");

	//Yeti Anim Class
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/PalWorld/Blueprints/YetiAnimation/ABP_PalYeti.ABP_PalYeti_C'"));
	if (tempAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimInstance.Class);
	}
	
}

// Called when the game starts or when spawned
void APalYeti::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//DataTable 로딩
		if (PalDataTable)
		{
			FPalInfoData* InfoData = PalDataTable->FindRow<FPalInfoData>(PalDataRowName, TEXT("Yeti"));
			if (InfoData)
			{
				YetiInfo = *InfoData;
			}
		}

		//팰 DataTable Data 초기화
		GetWorldTimerManager().SetTimer(TableDataTimerHandle, this, &APalYeti::SetTableData, 0.1f, false);

		//팰 Worker모드 시작 (임시)
		SetPalMode(EPalMode::Worker);
		//팰 모드별 상태 초기화
		SetPalWildState(EPalWildState::Patrol);
		SetPalBattleState(EPalBattleState::FollowPlayer);
		SetPalWorkerState(EPalWorkerState::Idle, nullptr);
	}

	//팰 현재 모드 출력
	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPalMode"), true);
	// if (EnumPtr)
	// {
	// 	FString EnumName = EnumPtr->GetNameStringByValue((int64)PalMode);
	// 	UE_LOG(PalLog, Warning, TEXT("[PalYeti, BeginPlay] EPalMode : %s"), *EnumName);
	// }
	
	//플레이어 소유 여부 (임시)
	bIsPlayerOwned = true;
	
	//팰 작업중 여부
	bIsWorking = false;

	//팰 회전
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	this->bUseControllerRotationYaw = false;
	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//애니메이션
	YetiAnimInstance = Cast<UPalYetiAnimInstance>(GetMesh()->GetAnimInstance());

	//팰 박스
	palBox = Cast<APalBox>(UGameplayStatics::GetActorOfClass(GetWorld(), APalBox::StaticClass()));

	IntervalDamage = 30.f;

}

// Called every frame
void APalYeti::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//팰 모드 분류
	switch (PalMode)
	{
		case EPalMode::Wild:
			SwitchWildState(DeltaTime);
			break;
		case EPalMode::Battle:
			SwitchBattleState();
			break;
		case EPalMode::Worker:
			SwitchWorkerState();
			break;
		case EPalMode::Carrier:
			break;
	}
}

// Called to bind functionality to input
void APalYeti::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APalYeti::SwitchWildState(float deltaTime)
{
	switch (PalWildState)
	{
		case EPalWildState::Patrol:
			HandleWildPatrol();
			break;
		case EPalWildState::PlayerHitToPal:
			HandleWildPlayerHitToPal();
			break;
		case EPalWildState::DetectPlayer:
			HandleWildDetectPlayer();
			break;
		case EPalWildState::Chase:
			HandleWildChase();
			break;
		case EPalWildState::Attack:
			HandleWildAttack(deltaTime);
			break;
		case EPalWildState::Escape:
			HandleWildEscape();
			break;
		case EPalWildState::Die:
			HandleWildDie();
			break;
	}
}

void APalYeti::SwitchBattleState()
{
	switch (PalBattleState)
	{
		case EPalBattleState::FollowPlayer:
			HandleBattleFollowPlayer();
			break;
		case EPalBattleState::DetectTarget:
			HandleBattleDetectTarget();
			break;
		case EPalBattleState::Chase:
			HandleBattleChase();
			break;
		case EPalBattleState::Attack:
			HandleBattleAttack();
			break;
		case EPalBattleState::Die:
			HandleBattleDie();
			break;
	}
}

void APalYeti::SwitchWorkerState()
{
	switch (PalWorkerState)
	{
		case EPalWorkerState::Idle:
			HandleWorkerIdle();
			break;
		case EPalWorkerState::FindWork:
			HandleWorkerFindWork();
			break;
		case EPalWorkerState::MoveToTarget:
			HandleWorkerMovetoTarget();
			break;
		case EPalWorkerState::Working:
			HandleWorkerWorking();
			break;
		case EPalWorkerState::Return:
			HandleWorkerReturn();
			break;
	}
}

void APalYeti::HandleWildPatrol()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildPatrol] WildState : Patrol Started"));
	
	//일정 범위 안에서 랜덤하게 이동하면서 순찰하기
	//네비게이션 시스템 받아오기
	UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NaviSystem)
	{
		return;
	}

	APWAIController* MyController = Cast<APWAIController>(GetController());
	if (!MyController)
	{
		return;
	}
	if (!bIsPatroling)
	{
		//지정해둔 범위 내에 랜덤 위치 받아오기
		FNavLocation RandomPoint;
		//첫 시작 지점을 기준으로 지정
		InitLocation = this->GetActorLocation();
		bool bFound = NaviSystem->GetRandomReachablePointInRadius(InitLocation, PatrolRadius,RandomPoint);
		if (bFound)
		{
			//타겟 지정해서 저장, 애니메이션 실행, 팰 목표 장소 이동
			CurrentPatrolTargetLocation = RandomPoint.Location;
			bIsPatroling = true;
			MultiRPC_WorkerIdle(bIsPatroling);
			//YetiAnimInstance->bIsPatroling = this->bIsPatroling;
			this->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
			MyController->MoveToLocation(CurrentPatrolTargetLocation);
			
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol bIsPatroling = %d"), bIsPatroling);
		}
	}
	//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	{
		//애니메이션 변경 및 다음 목표 위치로 이동하도록
		bIsPatroling = false;
		MultiRPC_WorkerIdle(bIsPatroling);
		//YetiAnimInstance->bIsPatroling = this->bIsPatroling;
		//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol Reached TargetLocation"));
	}

	//UE_LOG(PalYeti, Warning, TEXT("[HandleWildPatrol] Player Distance = %f"), FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()));
	if (player)
	{
		if (FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()) < PlayerDetectDistance && CurHP > 30)
		{
			//DetectPlayer 상태 변경
			SetPalWildState(EPalWildState::DetectPlayer);
		}
		//체력이 낮은데 플레이어가 다가오면 다시 도망
		if (FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()) < AttackDistance && CurHP <= 30 && CurHP > 0)
		{
			SetPalWildState(EPalWildState::Escape);
		}
	}

}

void APalYeti::HandleWildPlayerHitToPal()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildPlayerHitToPal] pal Name : %s WildState : PlayerHitToPal Started"), *this->GetName());

	//공격 중인 애니메이션 취소
	if (bIsPlayingAttackAnim)
	{
		bIsPlayingAttackAnim = false;
		//YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
	}
	//데미지 30씩 받을 때마다 데미지 애니메이션 실행
	if (!this->bIsDamaged)
	{
		this->bIsDamaged = true;
		//YetiAnimInstance->bIsDamaged = this->bIsDamaged;
	}
	MultiRPC_WildPlayerHitToPal();
}

void APalYeti::MultiRPC_WildPlayerHitToPal_Implementation()
{
	YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
	YetiAnimInstance->bIsDamaged = this->bIsDamaged;
}

void APalYeti::OnRep_Damaged()
{
	Super::OnRep_Damaged();
	YetiAnimInstance->bIsDamaged = this->bIsDamaged;
}

void APalYeti::OnRep_AttackAnim()
{
	Super::OnRep_AttackAnim();
	YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
}

void APalYeti::HandleWildDetectPlayer()
{
	//UE_LOG(PalYeti, Warning, TEXT("[HandleWildDetectPlayer] WildState : DetectPlayer"));

	//Escape 상태 변경
	SetPalWildState(EPalWildState::Chase);
}

void APalYeti::HandleWildChase()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildChase] WorkerState : Chase, Pal Name : %s"), *this->GetName());


	
	//Target 플레이어로 이동하기
	FVector meLoc = this->GetActorLocation();
	FVector playerLoc = player->GetActorLocation();
	
	//AIController Move To 
	APWAIController* MyAIController = Cast<APWAIController>(GetController());
	if (MyAIController)
	{
		//if (!bIsMoveToTarget)
		//{
			if (player)
			{
				this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
				MyAIController->MoveToLocation(playerLoc);
				bIsPatroling = false;
				MultiRPC_WildChasePatrol();
				//YetiAnimInstance->bIsPatroling = bIsPatroling;
				bIsMoveToTarget = true;
				MultiRPC_WildChaseMoveToTarget();
				//YetiAnimInstance->bIsMove = bIsMoveToTarget;
			}
		//}
	}

	//플레이어와 거리가 감지 범위보다 멀어지면
	if (FVector::DistXY(meLoc, playerLoc) > PlayerDetectDistance)
	{
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		MultiRPC_WildChaseMoveToTarget();
		//YetiAnimInstance->bIsMove = bIsMoveToTarget;
		//patrol 상태로 변경
		SetPalWildState(EPalWildState::Patrol);
	}
	
	//거리가 150보다 작으면 Attack State 시작
	if (FVector::DistXY(meLoc, playerLoc) < AttackDistance)
	{
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		MultiRPC_WildChaseMoveToTarget();
		//YetiAnimInstance->bIsMove = bIsMoveToTarget;
	
		//이동 정지 및 공격 상태 시작
		if (MyAIController)
		{
			MyAIController->StopMovement();
		}
		SetPalWildState(EPalWildState::Attack);
		
	}
}

void APalYeti::MultiRPC_WildChasePatrol_Implementation()
{
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsPatroling = bIsPatroling;
	}
}

void APalYeti::MultiRPC_WildChaseMoveToTarget_Implementation()
{
	YetiAnimInstance->bIsMove = bIsMoveToTarget;
}

void APalYeti::HandleWildAttack(float deltaTime)
{
	
	//공격 애니메이션 시작
	if (!bIsPlayingAttackAnim)
	{
		bIsPlayingAttackAnim = true;
		YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
	}

	//공격 범위 넘어가면 다시 chase 상태 전환
	if (FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()) > AttackDistance)
	{
		if (bIsPlayingAttackAnim)
		{
			bIsPlayingAttackAnim = false;
			YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
		}
		SetPalWildState(EPalWildState::Chase);
	}

	//플레이어 방향으로 회전하기
	FVector Direction = player->GetActorLocation() - this->GetActorLocation();
	Direction.Z = 0.0f;
	FRotator playerRotation = Direction.Rotation();
	FRotator currentRotation = this->GetActorRotation();

	FRotator newRotation = FMath::RInterpTo(currentRotation, playerRotation, deltaTime, 5.f);
	SetActorRotation(newRotation);
}

void APalYeti::HandleWildEscape()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildEscape] WorkerState : Escape, Pal Name : %s"), *this->GetName());

	//실행중인 타이머 체크
	if (GetWorldTimerManager().IsTimerActive(EscapeTimerHandle))
	{
		return;
	}
	
	//공격 애니메이션 취소
	if (bIsPlayingAttackAnim)
	{
		bIsPlayingAttackAnim = false;
		MultiRPC_WildEscapeAttackAnim();
		//YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
	}
	//patrol 애니메이션 취소
	if (bIsPatroling)
	{
		bIsPatroling = false;
		MultiRPC_WildEscapePatrol();
		//YetiAnimInstance->bIsPatroling = bIsPatroling;
	}
	//이동 애니메이션 실행
	if (!bIsMoveToTarget)
	{
		bIsMoveToTarget = true;
		MultiRPC_WildEscapeMoveToTarget();
		//YetiAnimInstance->bIsMove = bIsMoveToTarget;
	}
	
	//도망 함수 타이머
	GetWorldTimerManager().SetTimer(EscapeTimerHandle, this, &APalYeti::UpdateEscapeLocation, 0.2f, true);
}

void APalYeti::MultiRPC_WildEscapeAttackAnim_Implementation()
{
	YetiAnimInstance->bIsAttacking = bIsPlayingAttackAnim;
}

void APalYeti::MultiRPC_WildEscapePatrol_Implementation()
{
	YetiAnimInstance->bIsPatroling = bIsPatroling;
}

void APalYeti::MultiRPC_WildEscapeMoveToTarget_Implementation()
{
	YetiAnimInstance->bIsMove = bIsMoveToTarget;
}

void APalYeti::UpdateEscapeLocation()
{

	
	FVector playerLoc = player->GetActorLocation();
	FVector meLoc = this->GetActorLocation();
	APWAIController* MyController = Cast<APWAIController>(GetController());

	//플레이어 반대방향
	FVector RunDir = (meLoc - playerLoc).GetSafeNormal();
	
	//플레이어가 쫓아오고 있을 때 약간 좌/우로 틀기
	float RandomAngle = FMath::FRandRange(-45.0f, 45.0f);
	FVector TargetDir = RunDir.RotateAngleAxis(RandomAngle, FVector::UpVector);
	FRotator NewRotator = FMath::RInterpTo(RunDir.Rotation(), TargetDir.Rotation(), GetWorld()->GetDeltaSeconds(), 1.f);
	RunDir = NewRotator.Vector();
	
	//목표 도망 위치
	FVector TargetLoc = meLoc + RunDir * 700.f;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLoc;
	//목표 위치로 이동
	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(TargetLoc, 300.f, NavLoc))
	{
		this->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		MyController->MoveToLocation(NavLoc.Location);
	}
	//플레이어 감지 범위를 넘어서면 patrol 상태 전환
	if (FVector::DistXY(playerLoc, meLoc) > PlayerDetectDistance && CurHP > 0)
	{
		GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
		if(bIsMoveToTarget)
		{
			bIsMoveToTarget = false;
			MultiRPC_WildEscapeMoveToTarget();
			//YetiAnimInstance->bIsMove = bIsMoveToTarget;
		}
		SetPalWildState(EPalWildState::Patrol);
	}
	if (CurHP <= 0)
	{
		SetPalWildState(EPalWildState::Die);
	}
}

void APalYeti::HandleWildDie()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildDie] WorkerState : Die, Pal Name : %s"), *this->GetName());
	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
	APWAIController* MyController = Cast<APWAIController>(GetController());
	if (MyController)
	{
		MyController->StopMovement();
	}
	if(bIsMoveToTarget)
	{
		bIsMoveToTarget = false;
		MultiRPC_WildEscapeMoveToTarget();
		//YetiAnimInstance->bIsMove = bIsMoveToTarget;
	}
	
	//RagDoll
	//물리 시뮬레이션 활성화
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	//캡슐 콜라이더 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//컨트롤러 제거
	DetachFromControllerPendingDestroy();

	MultiRPC_WildDie();
	
	// //튕겨나가는 힘 추가
	// if (LaunchImpulse == FVector::ZeroVector)
	// {
	// 	//UE_LOG(LogTemp, Warning, TEXT("Launch Impulse"));
	// 	LaunchImpulse = GetActorForwardVector() * 250.f + FVector(0, 0, 500.f);
	// 	GetMesh()->AddImpulseToAllBodiesBelow(LaunchImpulse, NAME_None, true);
	// }

	
	// FVector ImpulseDir = FVector::ForwardVector;
	// if (player)
	// {
	// 	ImpulseDir = (this->GetActorLocation() - player->GetActorLocation()).GetSafeNormal();
	// 	ImpulseDir += FVector(0,0, 0.6f);
	// 	ImpulseDir.Normalize();
	// }
	//
	// FVector LaunchImpulse = ImpulseDir * 500.f;
	//
	// GetMesh()->AddImpulseToAllBodiesBelow(LaunchImpulse);
	//
	// GetWorldTimerManager().SetTimer(RagDollTimerHandle, this, &APalYeti::StopRagDollPhysics, 1.f, false);
	
}

// void APalYeti::StopRagDollPhysics()
// {
// 	// 물리 멈춤
// 	GetMesh()->SetSimulatePhysics(false);
// 	GetMesh()->SetAllBodiesSimulatePhysics(false);
// 	GetMesh()->bBlendPhysics = false;
// }

void APalYeti::MultiRPC_WildDie_Implementation()
{
	//RagDoll
	//물리 시뮬레이션 활성화
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	//캡슐 콜라이더 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float APalYeti::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                           class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("this pal Name = %s, bIsWorkerMode = %d, bIsCarrierMode = %d"), *this->GetName(), bIsWorkerMode,bIsCarrierMode);
	//작업, 운반 팰은 데미지 0
	if (this->bIsWorkerMode || this->bIsCarrierMode)
	{
		return 0.0f;
	}
	
	damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("this pal Name = %s, damage = %f"), *this->GetName(), damage);
	//팰 체력 감소
	CurHP = FMath::Clamp(CurHP - damage, 0, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("this pal Name = %s, CurHP = %f"), *this->GetName(), CurHP);
	
	//데미지 받았을 때 상태 전환
	if (this->bIsWildMode)
	{
		//일정 체력 이하로 떨어지면 Escape 상태로 변경
		if (CurHP < 30 && CurHP > 0)
		{
			SetPalWildState(EPalWildState::Escape);
		}
		//데미지 IntervalDamage(30)씩 받을때마다 PlayerHitToPal로 변환
		if (LastHP - CurHP >= IntervalDamage)
		{
			LastHP = CurHP;
			UE_LOG(LogTemp, Warning, TEXT("this pal Name = %s, PlayerHitToPal"), *this->GetName());
			SetPalWildState(EPalWildState::PlayerHitToPal);
		}
		//체력이 없으면 사망 상태 전환
		if (CurHP <= 0)
		{
			SetPalWildState(EPalWildState::Die);
		}

	}
	
	return damage;
}

void APalYeti::HandleBattleFollowPlayer()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleBattleFollowPlayer] BattleState : FollowPlayer"));
	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
	//이동 애니메이션 취소
	if (!bIsMoveToTarget)
	{
		bIsMoveToTarget = true;
		YetiAnimInstance->bIsMove = bIsMoveToTarget;
	}
}

void APalYeti::HandleBattleDetectTarget()
{
}

void APalYeti::HandleBattleChase()
{
}

void APalYeti::HandleBattleAttack()
{
}

void APalYeti::HandleBattleDie()
{
}

void APalYeti::HandleWorkerIdle()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerIdle] WorkerState : Idle Started"));
	
	//일정 범위 안에서 랜덤하게 이동하면서 순찰하기
	//네비게이션 시스템 받아오기
	 UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	 if (!NaviSystem)
	 {
	 	return;
	 }
	
	 APWAIController* MyController = Cast<APWAIController>(GetController());
	 if (!MyController)
	 {
	 	return;
	 }
	 if (!bIsPatroling)
	 {
	 	//지정해둔 범위 내에 랜덤 위치 받아오기
	 	FNavLocation RandomPoint;
	 	//첫 시작 지점을 기준으로 지정
	 	InitLocation = this->GetActorLocation();
	 	bool bFound = NaviSystem->GetRandomReachablePointInRadius(InitLocation, PatrolRadius,RandomPoint);
	 	if (bFound)
	 	{
	 		//타겟 지정해서 저장, 애니메이션 실행, 팰 목표 장소 이동
	 		CurrentPatrolTargetLocation = RandomPoint.Location;
	 		bIsPatroling = true;
	 		MultiRPC_WorkerIdle(bIsPatroling);
	 		//YetiAnimInstance->bIsPatroling = this->bIsPatroling;
	 		this->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	 		MyController->MoveToLocation(CurrentPatrolTargetLocation);
	 		
	 		//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
	 		//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol bIsPatroling = %d"), bIsPatroling);
	 	}
	 }
	 //UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	 //목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	 if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	 {
	 	//애니메이션 변경 및 다음 목표 위치로 이동하도록
	 	bIsPatroling = false;
	 	MultiRPC_WorkerIdle(bIsPatroling);
	 	//YetiAnimInstance->bIsPatroling = this->bIsPatroling;
	 	//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol Reached TargetLocation"));
	 }

	//ServerRPC_WorkerIdle();
}

//클라에서 WorkerIdle
void APalYeti::MultiRPC_WorkerIdle_Implementation(bool isPatrol)
{
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsPatroling = isPatrol;
	}
}

void APalYeti::HandleWorkerFindWork()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerFindWork] WorkerState : FindWork, WorkerPalName : %s"), *this->GetName());

	//타겟 자원이 있다면
	if (TargetResource)
	{
		bIsPatroling = false;
		MultiRPC_WorkerFindWork(bIsPatroling);
		//YetiAnimInstance->bIsPatroling = this->bIsPatroling;
		SetPalWorkerState(EPalWorkerState::MoveToTarget, TargetResource);
	}
	
	//ServerRPC_WorkerFindWork();
}

//클라에서 FindWork
void APalYeti::MultiRPC_WorkerFindWork_Implementation(bool isPatrol)
{
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsPatroling = isPatrol;
	}
}

void APalYeti::HandleWorkerMovetoTarget()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerMovetoTarget] WorkerState : MovetoTarget, WorkerPalName : %s"), *this->GetName());

	//Target 자원으로 이동하기
	 FVector meLoc = this->GetActorLocation();
	 FVector targetLoc = TargetResource->GetActorLocation();
	 //FVector dir = (targetLoc - meLoc).GetSafeNormal2D();
	
	 //this->SetActorLocation(meLoc + dir * MoveSpeed * GetWorld()->GetDeltaSeconds());
	 //AddMovementInput(dir);
	
	//AIController Move To 
	 APWAIController* MyAIController = Cast<APWAIController>(GetController());
	 if (MyAIController)
	 {
	 	if (!bIsMoveToTarget)
	 	{
	 		if (TargetResource)
	 		{
	 			this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	 			MyAIController->MoveToLocation(targetLoc);
	 			bIsMoveToTarget = true;
	 			bIsPlayingWorkAnim = false;
	 			MultiRPC_WorkerMovetoTarget(bIsMoveToTarget, bIsPlayingWorkAnim);
	 			//YetiAnimInstance->bIsMove = bIsMoveToTarget;
	 		}
	 	}
	 }
	
	//거리가 150보다 작으면 Working State 시작
	if (FVector::DistXY(meLoc, targetLoc) < 150.f)
	{
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		YetiAnimInstance->bIsMove = bIsMoveToTarget;
	
		//이동 정지 및 작업 상태 시작
		MyAIController->StopMovement();
		SetPalWorkerState(EPalWorkerState::Working, TargetResource);
		
		//작업중 애니메이션 시작
		bIsPlayingWorkAnim = true;
		MultiRPC_WorkerMovetoTarget(bIsMoveToTarget, bIsPlayingWorkAnim);
		//YetiAnimInstance->bIsWorking = bIsPlayingWorkAnim;
	
		//ServerRPC_WorkerMovetoTarget();
	}
	
}

//클라에서 팰 움직임 정지 및 상태 변환 
void APalYeti::MultiRPC_WorkerMovetoTarget_Implementation(bool isMove, bool isWork)
{
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsMove = isMove;
		YetiAnimInstance->bIsWorking = isWork;
	}

	//UE_LOG(PalYeti, Warning, TEXT("Role : %s, [PalYeti, MultiRPC_WorkerMovetoTarget_Implementation]  YetiAnimInstance->bIsMove : %d, YetiName : %s"), *UEnum::GetValueAsString<ENetRole>(GetLocalRole()), isMove, *this->GetName());
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, MultiRPC_WorkerMovetoTarget_Implementation]  YetiAnimInstance->bIsWorking : %d, YetiName : %s"), isWork, *this->GetName());

}

void APalYeti::OnRep_Patrol()
{
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsPatroling = bIsPatroling;
	}
}

void APalYeti::OnRep_MoveToTarget()
{
	Super::OnRep_MoveToTarget();
	if (YetiAnimInstance)
	{
		YetiAnimInstance->bIsMove = bIsMoveToTarget;
	}
}

void APalYeti::OnRep_WorkAnim()
{
	Super::OnRep_WorkAnim();
	YetiAnimInstance->bIsWorking = bIsPlayingWorkAnim;
}

void APalYeti::HandleWorkerWorking()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWorkerWorking] WorkerState : Working, WorkerPalName : %s"), *this->GetName());
	if (!TargetResource)
	{
		return;
	}
	//이미 타이머가 실행중이면 중복 실행 방지
	if (GetWorldTimerManager().IsTimerActive(WorkTimerHandle))
	{
		return;
	}
	//팰 작업속도
	if (ATree* tree = Cast<ATree>(TargetResource))
	{
		WorkSpeed = *YetiInfo.WorkSpeeds.Find("Tree");
	}
	else if (ARock* rock = Cast<ARock>(TargetResource))
	{
		WorkSpeed = *YetiInfo.WorkSpeeds.Find("Rock");
	}
	
	WorkInterval = 1.f / WorkSpeed;
	//WorkInterval마다 working 타이머 반복 실행
	GetWorldTimerManager().SetTimer(WorkTimerHandle, this, &APalYeti::PalWorking, WorkInterval, true);

	//ServerRPC_WorkerWorking();
	
}

void APalYeti::MultiRPC_WorkerWorking_Implementation()
{
	YetiAnimInstance->bIsWorking = bIsPlayingWorkAnim;
}

void APalYeti::PalWorking()
{
	Super::PalWorking();
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] WorkerState : Working, WorkerPalName : %s"), *this->GetName());

	//자원에 데미지 10씩 주기
	UGameplayStatics::ApplyDamage(TargetResource, 10.f, GetController(), this, nullptr);
	
	//데미지 주다가 자원이 비활성화 되면, 자원과 팰 둘다 작업상태 멈춤 Rest상태로 Rest 배열에 자동 추가됨(자원은 재생성될때)
	if (ATree* tree = Cast<ATree>(TargetResource))
	{
		//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] tree->IsActive() : %d, tree->IsBeingWorkedOn() : %d,  WorkerPalName : %s"), tree->IsActive(), tree->IsBeingWorkedOn(), *this->GetName());
		if (tree && !tree->IsActive())
		{
			//자원과 팰 둘다 작업상태 멈춤
			tree->SetIsBeingWorkedOn(false);
			this->SetPalIsWorking(false);

			//작업중 애니메이션 취소
			bIsPlayingWorkAnim = false;
			MultiRPC_WorkerWorking();
			// YetiAnimInstance->bIsWorking = false;

			//팰 상태 Return으로 초기화
			this->SetPalWorkerState(EPalWorkerState::Return, nullptr);
		}
	}
	if (ARock* rock = Cast<ARock>(TargetResource))
	{
		//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] rock->IsActive() : %d, rock->IsBeingWorkedOn() : %d, WorkerPalName : %s"), rock->IsActive(), rock->IsBeingWorkedOn(), *this->GetName());
		if (rock && !rock->IsActive())
		{
			//자원과 팰 둘다 작업상태 멈춤
			rock->SetIsBeingWorkedOn(false);
			this->SetPalIsWorking(false);

			//작업중 애니메이션 취소
			bIsPlayingWorkAnim = false;
			MultiRPC_WorkerWorking();
			//YetiAnimInstance->bIsWorking = false;

			//팰 상태 Return으로 초기화
			this->SetPalWorkerState(EPalWorkerState::Return, nullptr);
			
		}
	}
}

void APalYeti::HandleWorkerReturn()
{
	//Work Timer 돌고 있으면 클리어
	if (GetWorldTimerManager().IsTimerActive(WorkTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(WorkTimerHandle);
	}
	//Idle 상태로 변경
	this->SetPalWorkerState(EPalWorkerState::Idle, nullptr);

	//ServerRPC_WorkerReturn();
}

void APalYeti::SetTableData()
{
	Super::SetTableData();

	//팰 속도
	MoveSpeed = YetiInfo.MoveSpeed;
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	//팰 순찰속도
	PatrolSpeed = YetiInfo.PatrolSpeed;
	
	//팰 도망속도
	RunSpeed = YetiInfo.RunSpeed;
	
	//팰 체력
	MaxHP = YetiInfo.HP;
	CurHP = MaxHP;
	LastHP = MaxHP;
	
}

void APalYeti::SetPalMode(EPalMode Mode)
{
	Super::SetPalMode(Mode);

	if (Mode == EPalMode::Wild)
	{
		PalMode = EPalMode::Wild;
		
		bIsWildMode = true;
		bIsBattleMode = false;
		bIsWorkerMode = false;
	}
	else if (Mode == EPalMode::Battle)
	{
		PalMode = EPalMode::Battle;
		
		bIsWildMode = false;
		bIsBattleMode = true;
		bIsWorkerMode = false;
	}
	else if (Mode == EPalMode::Worker)
	{
		PalMode = EPalMode::Worker;
		
		bIsWildMode = false;
		bIsBattleMode = false;
		bIsWorkerMode = true;
	}
}

void APalYeti::SetPalWildState(EPalWildState State)
{
	Super::SetPalWildState(State);

	PalWildState = State;
}

void APalYeti::SetPalBattleState(EPalBattleState State)
{
	Super::SetPalBattleState(State);

	PalBattleState = State;
}

void APalYeti::SetPalWorkerState(EPalWorkerState State, AActor* TargetActor)
{
	Super::SetPalWorkerState(State, TargetActor);

	PalWorkerState = State;
	//Target이 있다면
	if (TargetActor)
	{
		TargetResource = TargetActor;
	}
}

bool APalYeti::GetPalIsWorking() const
{
	return Super::GetPalIsWorking();
}

void APalYeti::SetPalIsWorking(bool IsWorking)
{
	Super::SetPalIsWorking(IsWorking);

	bIsWorking = IsWorking;
}

void APalYeti::CaptureByPlayer()
{
	//팰이 사라지면 전체 팰 배열에서 제거
	if (palBox->AllPalsInMap.Contains(this))
	{
		palBox->AllPalsInMap.Remove(this);
		UE_LOG(PalYeti, Warning, TEXT("[CaptureByPlayer]Removed AllPalsInMap Actor : %s"), *this->GetName());
	}
	
	Super::CaptureByPlayer();
}

void APalYeti::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APalYeti, bIsPatroling);
	//DOREPLIFETIME(APalYeti, GetMesh());
}