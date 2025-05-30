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
	Carrier UMETA(DisplayName = "Carrier"),
};

UENUM(BlueprintType)
enum class EPalWildState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	PlayerHitToPal UMETA(DisplayName = "PlayerHitToPal"),
	DetectPlayer UMETA(DisplayName = "DetectPlayer"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplaName = "Attack"),
	Escape UMETA(DisplayName = "Escape"),
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

UENUM(BlueprintType)
enum class EPalCarrierState : uint8
{
	Patrol UMETA(DisplayName = "Patrol"),
	FindItem UMETA(DisplayName = "FindItem"),
	MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	Carrying UMETA(DisplayName = "Carrying"),
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	EPalCarrierState PalCarrierState;
	
	//팰 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal")
	FPalInfoData YetiInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pal")
	FPalInfoData ChickenInfo;

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
	// 팰 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;
	//팰 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* WorkMontage;
	
	//팰 모드 구분
	bool bIsWildMode = false;
	bool bIsBattleMode = false;
	bool bIsWorkerMode = false;
	bool bIsCarrierMode = false;

	//팰 이동중 여부
	bool bIsMoveToTarget = false;
	
	//팰 작업중 여부
	bool bIsWorking = false;

	//팰 작업 애니메이션 실행 여부
	bool bIsPlayingWorkAnim = false;

	//팰 공격 애니메이션 실행 여부
	bool bIsPlayingAttackAnim = false;
	
	//팰 운반중 여부
	bool bIsCarrying = false;



	//작업할 자원
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetResource = nullptr;
	//작업할 아이템
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetItem = nullptr;

	//작업 간격
	float WorkInterval = 0.0f;

	//플레이어 소유여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPlayerOwned = false;

	//플레이어
	class APlayerCharacter* player;
	
	//팰 작업중 get set
	virtual bool GetPalIsWorking() const { return bIsWorking; }
	virtual void SetPalIsWorking(bool IsWorking);
	//팰 운반중 get set
	virtual bool GetPalIsCarrying() const { return bIsCarrying; }
	virtual void SetPalIsCarrying(bool IsCarrying);

	//팰 DataTable 데이터 받아오기 함수
	virtual void SetTableData();

	//팰 작업 실행 함수
	virtual void PalWorking();

	//팰 운반 실행 함수
	virtual void PalCarrying();
	
	//팰 모드 Set함수
	virtual void SetPalMode(EPalMode Mode);

	//팰 모드별 상태 Set함수
	virtual void SetPalWildState(EPalWildState State);
	virtual void SetPalBattleState(EPalBattleState State);
	virtual void SetPalWorkerState(EPalWorkerState State, AActor* TargetActor);
	virtual void SetPalCarrierState(EPalCarrierState State, AActor* TargetActor);
	
	//팰이 잡혔을때 함수
	void CaptureByPlayer();
	
	//현재 상태 업데이트
	virtual void UpdateFSM(float DeltaTime);
};
