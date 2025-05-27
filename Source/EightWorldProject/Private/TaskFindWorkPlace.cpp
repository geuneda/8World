// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskFindWorkPlace.h"

#include "PalYeti.h"
#include "PalWorkComponent.h"
#include "PWAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UTaskFindWorkPlace::UTaskFindWorkPlace()
{
}

EBTNodeResult::Type UTaskFindWorkPlace::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APWAIController* AIController = Cast<APWAIController>(OwnerComp.GetAIOwner());
	APalYeti* Pal = Cast<APalYeti>(AIController->GetCharacter());

	if (!Pal)
	{
		return EBTNodeResult::Failed;
	}

	UPalWorkComponent* PalWorkComp = Pal->PalworkComp;
	if (!PalWorkComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetWorkPlace = PalWorkComp->FindAvailableWorkPlaceNearby();
	if (!TargetWorkPlace)
	{
		return EBTNodeResult::Failed;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject("TargetWorkPlace", TargetWorkPlace);
	return EBTNodeResult::Succeeded;
	
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
}
