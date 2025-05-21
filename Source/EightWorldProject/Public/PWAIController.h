// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PWAIController.generated.h"


UCLASS()
class EIGHTWORLDPROJECT_API APWAIController : public AAIController
{
	GENERATED_BODY()

public:
	APWAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnPossess(APawn* InPawn) override;

	//BT
	UPROPERTY(EditAnywhere, category = AI)
	class UBehaviorTree* PalBehaviorTree;
};
