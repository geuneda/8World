// Fill out your copyright notice in the Description page of Project Settings.


#include "PalAlpaca.h"

#include "PalWorkComponent.h"

DECLARE_LOG_CATEGORY_EXTERN(PalLog, Log, All);
DEFINE_LOG_CATEGORY(PalLog);

// Sets default values
APalAlpaca::APalAlpaca()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Alpaca Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Alpaca/Alpaca.Alpaca'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-11.f), FRotator(0,-90,0));
	}

	PalworkComp = CreateDefaultSubobject<UPalWorkComponent>(TEXT("PalWorkComp"));
	
}

// Called when the game starts or when spawned
void APalAlpaca::BeginPlay()
{
	Super::BeginPlay();

	//팰 Worker모드 시작 (임시)
	SetPalMode(EPalMode::Worker);
	
	//팰 현재 모드 출력
	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPalMode"), true);
	// if (EnumPtr)
	// {
	// 	FString EnumName = EnumPtr->GetNameStringByValue((int64)PalMode);
	// 	UE_LOG(PalLog, Warning, TEXT("[PalAlpaca, BeginPlay] EPalMode : %s"), *EnumName);
	// }
	
	//팰 모드별 상태 초기화
	SetPalWildState(EPalWildState::Patrol);
	SetPalBattleState(EPalBattleState::FollowPlayer);
	SetPalWorkerState(EPalWorkerState::Idle);

	//팰 작업중 여부
	bIsWorking = false;
}

// Called every frame
void APalAlpaca::Tick(float DeltaTime)
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
		case EPalMode::Worker:
			SwitchWorkerState();
			break;
	}
}

// Called to bind functionality to input
void APalAlpaca::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APalAlpaca::SwitchWildState()
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

void APalAlpaca::SwitchBattleState()
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

void APalAlpaca::SwitchWorkerState()
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

void APalAlpaca::HandleWildPatrol()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalAlpaca, HandleWildPatrol] WildState : Patrol"));
}

void APalAlpaca::HandleWildPlayerHitToPal()
{
}

void APalAlpaca::HandleWildDetectPlayer()
{
}

void APalAlpaca::HandleWildChase()
{
}

void APalAlpaca::HandleWildAttack()
{
}

void APalAlpaca::HandleWildReturn()
{
}

void APalAlpaca::HandleBattleFollowPlayer()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalAlpaca, HandleBattleFollowPlayer] BattleState : FollowPlayer"));
}

void APalAlpaca::HandleBattleDetectTarget()
{
}

void APalAlpaca::HandleBattleChase()
{
}

void APalAlpaca::HandleBattleAttack()
{
}

void APalAlpaca::HandleWorkerIdle()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalAlpaca, HandleWorkerIdle] WorkerState : Idle"));
}

void APalAlpaca::HandleWorkerFindWork()
{
}

void APalAlpaca::HandleWorkerMovetoTarget()
{
}

void APalAlpaca::HandleWorkerWorking()
{
}

void APalAlpaca::HandleWorkerReturn()
{
}

void APalAlpaca::SetPalMode(EPalMode Mode)
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

void APalAlpaca::SetPalWildState(EPalWildState State)
{
	Super::SetPalWildState(State);

	PalWildState = State;
}

void APalAlpaca::SetPalBattleState(EPalBattleState State)
{
	Super::SetPalBattleState(State);

	PalBattleState = State;
}

void APalAlpaca::SetPalWorkerState(EPalWorkerState State)
{
	Super::SetPalWorkerState(State);

	PalWorkerState = State;
}

bool APalAlpaca::GetPalIsWorking() const
{
	return Super::GetPalIsWorking();
}

void APalAlpaca::SetPalIsWorking(bool IsWorking)
{
	Super::SetPalIsWorking(IsWorking);

	bIsWorking = IsWorking;
}
