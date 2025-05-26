// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PalAlpacaAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API UPalAlpacaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPalAlpacaAnimInstance();

	virtual void NativeInitializeAnimation() override;

public:
	//팰 이동중인지 여부
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsMove;
};
