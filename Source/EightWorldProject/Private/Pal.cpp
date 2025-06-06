// Fill out your copyright notice in the Description page of Project Settings.


#include "Pal.h"

#include "PWAIController.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APal::APal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DataTable 추가하기
	//서버에서 로드
	if (HasAuthority())
	{
		UDataTable* LoadedTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, TEXT("'/Game/PalWorld/DataTables/DT_PalInfoData.DT_PalInfoData'")));
		if (LoadedTable)
		{
			PalDataTable = LoadedTable;
			UE_LOG(LogTemp, Warning, TEXT("DataTable 로드 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DataTable 로드 실패"));
		}
	}
	// ConstructorHelpers::FObjectFinder<UDataTable> tempDataTable(TEXT("'/Game/PalWorld/DataTables/DT_PalInfoData.DT_PalInfoData'"));
	// if (tempDataTable.Succeeded())
	// {
	// 	PalDataTable = tempDataTable.Object;
	// }

	//스폰 되었을 때도 자동으로 AIController 붙이기
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = APWAIController::StaticClass();

	//동기화 여부
	bReplicates = true;
}

// Called when the game starts or when spawned
void APal::BeginPlay()
{
	Super::BeginPlay();

	//서버에서 실행
	if (HasAuthority())
	{
		FTimerHandle FindPlayerTimerHandle;
		if (!GetWorldTimerManager().IsTimerActive(FindPlayerTimerHandle))
		{
			//주기적으로 가까운 플레이어 찾기
			GetWorldTimerManager().SetTimer(FindPlayerTimerHandle, this, &APal::SerachDelayPlayer, 1.0f, true);
		}
		
	}
}

void APal::SerachDelayPlayer()
{
	//플레이어들 중 가장 가까운 플레이어 찾기
	if (HasAuthority())
	{
		APlayerCharacter* ClosestPlayer = nullptr;
		float ClosestDist = 100000.f;

		TArray<AActor*> FoundPlayers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundPlayers);

		for (AActor* PlayerActor : FoundPlayers)
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerActor);
			if (PlayerCharacter && !PlayerCharacter->IsPendingKillPending())
			{
				float dist = FVector::Dist(PlayerActor->GetActorLocation(), this->GetActorLocation());
				if (dist < ClosestDist)
				{
					ClosestDist = dist;
					ClosestPlayer = PlayerCharacter;
				}
			}
		}
		
		player = ClosestPlayer;
		//UE_LOG(LogTemp, Warning, TEXT("ClosestPlayer : %s"), *player->GetName());
	}
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

//변수 동기화
void APal::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APal, PalDataTable);
	DOREPLIFETIME(APal, player);
	DOREPLIFETIME(APal, bIsMoveToTarget);
	DOREPLIFETIME(APal, bIsPlayingWorkAnim);
	DOREPLIFETIME(APal, bIsDamaged);
	DOREPLIFETIME(APal, bIsPlayingAttackAnim);
	DOREPLIFETIME(APal, CurHP);
	DOREPLIFETIME(APal, MaxHP);
	DOREPLIFETIME(APal, LastHP);
	DOREPLIFETIME(APal, damage);
}

