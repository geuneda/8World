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
};
