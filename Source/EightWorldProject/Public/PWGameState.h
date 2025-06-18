// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PWGameState.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API APWGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	APWGameState();

	virtual void BeginPlay() override;
	
	//공유되는 아이템개수
	UPROPERTY(ReplicatedUsing = OnRep_Count)
	int32 SharedItemCount;

	//UI 갱신용
	UFUNCTION()
	void OnRep_Count();

	//아이템개수 세팅
	void SetSharedItemCount(int32 NewCount);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
