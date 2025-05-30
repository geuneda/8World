// Fill out your copyright notice in the Description page of Project Settings.


#include "PWAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

APWAIController::APWAIController()
{
	//BT 생성
	ConstructorHelpers::FObjectFinder<UBehaviorTree> tempBT(TEXT("BehaviorTree'/Game/PalWorld/AI/BT_Pal.BT_Pal'"));
	if (tempBT.Succeeded())
	{
		PalBehaviorTree = tempBT.Object;
	}
}

void APWAIController::BeginPlay()
{
	Super::BeginPlay();
	
	//BT 시작
	if (PalBehaviorTree != nullptr)
	{
		RunBehaviorTree(PalBehaviorTree);
	}
}

void APWAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APWAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
