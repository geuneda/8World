#pragma once

#include "CoreMinimal.h"
#include "PalInfoData.generated.h"

enum class EPalMode : uint8;

USTRUCT(BlueprintType)
struct FPalInfoData : public FTableRowBase
{
	GENERATED_BODY()

	//팰 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PalName;

	//팰 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PalMode;

	//팰 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	//팰 순찰속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolSpeed;
	
	//팰 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	//팰 도망속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;
	
	//팰 작업종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> WorkTypes;

	//팰 작업별 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> WorkSpeeds;

	//팰 작업별 수확량
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> WorkAmounts;

	//팰 작업별 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, UAnimMontage*> WorkMontages;
};