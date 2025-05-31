// Fill out your copyright notice in the Description page of Project Settings.


#include "Pal.h"

#include "PWAIController.h"
#include "EightWorldProject/Player/PlayerCharacter.h"


// Sets default values
APal::APal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DataTable 추가하기
	ConstructorHelpers::FObjectFinder<UDataTable> tempDataTable(TEXT("'/Game/PalWorld/DataTables/DT_PalInfoData.DT_PalInfoData'"));
	if (tempDataTable.Succeeded())
	{
		PalDataTable = tempDataTable.Object;
	}

	//스폰 되었을 때도 자동으로 AIController 붙이기
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = APWAIController::StaticClass();
}

// Called when the game starts or when spawned
void APal::BeginPlay()
{
	Super::BeginPlay();
	
	//플레이어 찾기
	player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void APal::SetPalIsWorking(bool IsWorking)
{
}

void APal::SetPalIsCarrying(bool IsCarrying)
{
}

void APal::SetTableData()
{
}

void APal::PalWorking()
{
}

void APal::PalCarrying()
{
}

void APal::SetPalMode(EPalMode Mode)
{
}

void APal::SetPalWildState(EPalWildState State)
{
}

void APal::SetPalBattleState(EPalBattleState State)
{
}

void APal::SetPalWorkerState(EPalWorkerState State, AActor* TargetActor)
{
}

void APal::SetPalCarrierState(EPalCarrierState State, AActor* TargetActor)
{
}

void APal::CaptureByPlayer()
{
	//팰 파괴
	this->Destroy();
}

float APal::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator,
	AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void APal::UpdateFSM(float DeltaTime)
{
}

