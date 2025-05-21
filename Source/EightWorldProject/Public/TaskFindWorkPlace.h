// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TaskFindWorkPlace.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API UTaskFindWorkPlace : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTaskFindWorkPlace();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
	
};
