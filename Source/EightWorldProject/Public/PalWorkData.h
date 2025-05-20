#pragma once

#include "CoreMinimal.h"
#include "PalWorkData.generated.h"

USTRUCT(BlueprintType)
struct FPalWorkData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PalName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> WorkTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> WorkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> WorkMontage;
};