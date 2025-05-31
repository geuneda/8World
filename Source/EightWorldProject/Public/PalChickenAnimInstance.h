// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalChickenAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UPalChickenAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPalChickenAnimInstance();

	virtual void NativeInitializeAnimation() override;
	
public:
	//팰 순찰중인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPatroling;

	//팰 이동중인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving;

	//팰 데미지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDamaged;

	//소유자
	UPROPERTY()
	class APalChicken* Chicken;
	
	//Damage 애니메이션 Notify
	UFUNCTION()
	void AnimNotify_DamageAnimEnd();
};
