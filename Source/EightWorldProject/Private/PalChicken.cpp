// Fill out your copyright notice in the Description page of Project Settings.


#include "PalChicken.h"

#include "NavigationSystem.h"
#include "PalChickenAnimInstance.h"
#include "PWAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

APalChicken::APalChicken()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Chicken Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Chiken/Chiken.Chiken'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90.f), FRotator(0,-90,0));
	}

	//Chicken Anim Class
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/PalWorld/Blueprints/ChickenAnimation/ABP_PalChicken.ABP_PalChicken_C'"));
	if (tempAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimInstance.Class);
	}
}

void APalChicken::BeginPlay()
{
	Super::BeginPlay();

	//팰 Carrier모드 시작 (임시)
	SetPalMode(EPalMode::Carrier);

	//팰 모드별 상태 초기화
	SetPalWildState(EPalWildState::Patrol);
	SetPalBattleState(EPalBattleState::FollowPlayer);
	SetPalCarrierState(EPalCarrierState::Patrol, nullptr);

	//팰 운반중 여부
	bIsCarrying = false;

	//팰 회전
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	this->bUseControllerRotationYaw = false;
	this->GetCharacterMovement()->bOrientRotationToMovement = true;

	//애니메이션
	ChickenAnimInstance = Cast<UPalChickenAnimInstance>(GetMesh()->GetAnimInstance());
	
	//첫 시작 지점을 기준으로 지정
	InitLocation = this->GetActorLocation();

	//팰 DataTable Data 초기화
	GetWorldTimerManager().SetTimer(TableDataTimerHandle, this, &APalChicken::SetTableData, 0.2f, false);
}

void APalChicken::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//팰 모드 분류
	switch (PalMode)
	{
	case EPalMode::Wild:
		SwitchWildState();
		break;
	case EPalMode::Battle:
		SwitchBattleState();
		break;
	case EPalMode::Carrier:
		SwitchCarrierState();
		break;
	case EPalMode::Worker:
		break;
	}
}

void APalChicken::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APalChicken::SwitchWildState()
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
		HandleWildAttack();
		break;
	case EPalWildState::Return:
		HandleWildReturn();
		break;
	}
}

void APalChicken::SwitchBattleState()
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
	}
}

void APalChicken::SwitchCarrierState()
{
	switch (PalCarrierState)
	{
	case EPalCarrierState::Patrol:
		HandleCarrierIdle();
		break;
	case EPalCarrierState::FindItem:
		HandleCarrierFindItem();
		break;
	case EPalCarrierState::MoveToTarget:
		HandleCarrierMovetoTarget();
		break;
	case EPalCarrierState::Carrying:
		HandleCarrierCarrying();
		break;
	case EPalCarrierState::Return:
		HandleCarrierReturn();
		break;
	}
}

void APalChicken::HandleWildPatrol()
{
}

void APalChicken::HandleWildPlayerHitToPal()
{
}

void APalChicken::HandleWildDetectPlayer()
{
}

void APalChicken::HandleWildChase()
{
}

void APalChicken::HandleWildAttack()
{
}

void APalChicken::HandleWildReturn()
{
}

void APalChicken::HandleBattleFollowPlayer()
{
}

void APalChicken::HandleBattleDetectTarget()
{
}

void APalChicken::HandleBattleChase()
{
}

void APalChicken::HandleBattleAttack()
{
}

void APalChicken::HandleCarrierIdle()
{
	//일정 범위 안에서 랜덤하게 이동하면서 순찰하기
	//네비게이션 시스템 받아오기
	UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NaviSystem)
	{
		return;
	}

	//지정해둔 범위 내에 랜덤 위치 받아오기
	FNavLocation RandomPoint;
	bool bFound = NaviSystem->GetRandomReachablePointInRadius(InitLocation, PatrolRadius,RandomPoint);
	if (bFound)
	{
		APWAIController* MyController = Cast<APWAIController>(GetController());
		if (MyController)
		{
			if (!bIsPatroling)
			{
				bIsPatroling = true;
				MyController->MoveToLocation(RandomPoint.Location);
			}
		}
	}

	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기
	if (FVector::DistXY(this->GetActorLocation(), RandomPoint.Location) < 5.f)
	{
		bIsPatroling = false;
	}
	
}

void APalChicken::HandleCarrierFindItem()
{
}

void APalChicken::HandleCarrierMovetoTarget()
{
}

void APalChicken::HandleCarrierCarrying()
{
}

void APalChicken::HandleCarrierReturn()
{
}

inline void APalChicken::SetTableData()
{
	Super::SetTableData();

	//팰 속도
	MoveSpeed = ChickenInfo.MoveSpeed;
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}


void APalChicken::SetPalMode(EPalMode Mode)
{
	Super::SetPalMode(Mode);

	if (Mode == EPalMode::Wild)
	{
		PalMode = EPalMode::Wild;
		
		bIsWildMode = true;
		bIsBattleMode = false;
		bIsCarrierMode = false;
	}
	else if (Mode == EPalMode::Battle)
	{
		PalMode = EPalMode::Battle;
		
		bIsWildMode = false;
		bIsBattleMode = true;
		bIsCarrierMode = false;
	}
	else if (Mode == EPalMode::Carrier)
	{
		PalMode = EPalMode::Carrier;
		
		bIsWildMode = false;
		bIsBattleMode = false;
		bIsCarrierMode = true;
	}
}

void APalChicken::SetPalWildState(EPalWildState State)
{
	Super::SetPalWildState(State);

	PalWildState = State;
}

void APalChicken::SetPalBattleState(EPalBattleState State)
{
	Super::SetPalBattleState(State);
	
	PalBattleState = State;
}

void APalChicken::SetPalCarrierState(EPalCarrierState State, AActor* TargetActor)
{
	Super::SetPalCarrierState(State, TargetActor);

	PalCarrierState = State;
	//Target이 있다면
	if (TargetActor)
	{
		TargetItem = TargetActor;
	}
}

bool APalChicken::GetPalIsCarrying() const
{
	return Super::GetPalIsCarrying();
}

void APalChicken::SetPalIsCarrying(bool IsCarrying)
{
	Super::SetPalIsCarrying(IsCarrying);

	bIsCarrying = IsCarrying;
}