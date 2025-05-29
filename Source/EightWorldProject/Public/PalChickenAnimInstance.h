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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPatroling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsMoving;
};
