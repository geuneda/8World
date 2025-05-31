// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalYetiAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UPalYetiAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPalYetiAnimInstance();

	virtual void NativeInitializeAnimation() override;

public:
	//팰 이동중인지 여부
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsMove;

	//팰 작업중인지 여부
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsWorking;

	//팰 순찰중인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPatroling;

	//팰 공격중인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttacking;

	//팰 데미지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDamaged;

	//소유자
	UPROPERTY()
	class APalYeti* yeti;
	
	//Damage 애니메이션 Notify
	UFUNCTION()
	void AnimNotify_DamageEnd();
};
