// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pal.h"
#include "PalChicken.generated.h"


UCLASS()
class EIGHTWORLDPROJECT_API APalChicken : public APal
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APalChicken();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//팰 에니메이션
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UPalChickenAnimInstance* ChickenAnimInstance;

	//팰 모드 Set함수
	virtual void SetPalMode(EPalMode Mode) override;

	//팰 모드별 상태 Set함수
	virtual void SetPalWildState(EPalWildState State) override;
	virtual void SetPalBattleState(EPalBattleState State) override;
	virtual void SetPalCarrierState(EPalCarrierState State, AActor* TargetActor) override;

	//팰 운반중 get set
	virtual bool GetPalIsCarrying() const override;
	virtual void SetPalIsCarrying(bool IsCarrying) override;

	//팰 모드 스위치 함수
	void SwitchWildState();
	void SwitchBattleState();
	void SwitchCarrierState();
	
	//팰 Wild 모드 함수
	void HandleWildPatrol();
	void HandleWildPlayerHitToPal();
	void HandleWildDetectPlayer();
	void HandleWildChase();
	void HandleWildAttack();
	void HandleWildReturn();

	//팰 Battle 모드 함수
	void HandleBattleFollowPlayer();
	void HandleBattleDetectTarget();
	void HandleBattleChase();
	void HandleBattleAttack();

	//팰 Carrier 모드 함수
	void HandleCarrierIdle();
	void HandleCarrierFindItem();
	void HandleCarrierMovetoTarget();
	void HandleCarrierCarrying();
	void HandleCarrierReturn();
};
