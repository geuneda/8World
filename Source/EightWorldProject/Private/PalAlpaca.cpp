// Fill out your copyright notice in the Description page of Project Settings.


#include "PalAlpaca.h"

#include "PalWorkComponent.h"

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

