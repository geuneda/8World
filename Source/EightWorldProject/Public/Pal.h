// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PalInfoData.h"
#include "GameFramework/Character.h"
#include "Pal.generated.h"


UENUM(BlueprintType)
enum class EPalMode : uint8
{
	Wild UMETA(DisplayName = "Wild"),
	Battle UMETA(DisplayName = "Battle"),
	Worker UMETA(DisplayName = "Worker"),
};

UENUM(BlueprintType)
enum class EPalWildState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	PlayerHitToPal UMETA(DisplayName = "PlayerHitToPal"),
	DetectPlayer UMETA(DisplayName = "DetectPlayer"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplaName = "Attack"),
	Return UMETA(DisplayName = "Return"),
};

UENUM(BlueprintType)
enum class EPalBattleState : uint8
{
	FollowPlayer UMETA(DisplayName = "FollowPlayer"),
	DetectTarget UMETA(DisplayName = "DetectTarget"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisPlayName = "Attack"),
};

UENUM(BlueprintType)
enum class EPalWorkerState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	FindWork UMETA(DisplayName = "FindWork"),
	MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	Working UMETA(DisplayName = "Working"),
	Return UMETA(DisplayName = "Return"),
};


UCLASS()
class EIGHTWORLDPROJECT_API APal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//팰 모드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	EPalMode PalMode;

	//팰 상태들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	EPalWildState PalWildState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	EPalBattleState PalBattleState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	EPalWorkerState PalWorkerState;
	
	//팰 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	FPalInfoData PalInfo;

	//펠 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	FName PalDataRowName;

	//펠 DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalData")
	UDataTable* PalDataTable;
	
	//팰 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PalName;
	//팰 일종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WorkType;
	//팰 일속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorkSpeed;
	//팰 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* WorkMontage;


	//현재 상태 업데이트
	virtual void UpdateFSM(float DeltaTime);
};
