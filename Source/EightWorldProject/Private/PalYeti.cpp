// Fill out your copyright notice in the Description page of Project Settings.


#include "PalYeti.h"

#include "NavigationSystem.h"
#include "PalYetiAnimInstance.h"
#include "PalBox.h"
#include "PalWorkComponent.h"
#include "PWAIController.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/Resources/Rock.h"
#include "EightWorldProject/Resources/Tree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DECLARE_LOG_CATEGORY_EXTERN(PalYeti, Log, All);
DEFINE_LOG_CATEGORY(PalYeti);

// Sets default values
APalYeti::APalYeti()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Yeti Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Pal/Yeti/Yeti.Yeti'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeScale3D(FVector(0.4f));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90.f), FRotator(0,-90,0));
	}

	PalworkComp = CreateDefaultSubobject<UPalWorkComponent>(TEXT("PalWorkComp"));

	//DataTable RowName 초기화
	PalDataRowName = TEXT("Yeti");

	//Yeti Anim Class
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/PalWorld/Blueprints/YetiAnimation/ABP_PalYeti.ABP_PalYeti_C'"));
	if (tempAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimInstance.Class);
	}
}

// Called when the game starts or when spawned
void APalYeti::BeginPlay()
{
	Super::BeginPlay();

	//DataTable 로딩
	if (PalDataTable)
	{
		FPalInfoData* InfoData = PalDataTable->FindRow<FPalInfoData>(PalDataRowName, TEXT("Yeti"));
		if (InfoData)
		{
			YetiInfo = *InfoData;
		}
	}

	//플레이어 소유 여부 (임시)
	bIsPlayerOwned = true;
	
	//팰 Worker모드 시작 (임시)
	SetPalMode(EPalMode::Worker);
	
	//팰 현재 모드 출력
	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPalMode"), true);
	// if (EnumPtr)
	// {
	// 	FString EnumName = EnumPtr->GetNameStringByValue((int64)PalMode);
	// 	UE_LOG(PalLog, Warning, TEXT("[PalYeti, BeginPlay] EPalMode : %s"), *EnumName);
	// }
	
	//팰 모드별 상태 초기화
	SetPalWildState(EPalWildState::Patrol);
	SetPalBattleState(EPalBattleState::FollowPlayer);
	SetPalWorkerState(EPalWorkerState::Idle, nullptr);

	//팰 작업중 여부
	bIsWorking = false;

	//팰 회전
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	this->bUseControllerRotationYaw = false;
	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	
	//팰 DataTable Data 초기화
	GetWorldTimerManager().SetTimer(TableDataTimerHandle, this, &APalYeti::SetTableData, 0.1f, false);

	//애니메이션
	YetiAnimInstance = Cast<UPalYetiAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called every frame
void APalYeti::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//팰 모드 분류
	switch (PalMode)
	{
		case EPalMode::Wild:
			SwitchWildState();
			break;
		case EPalMode::Battle:
			SwitchBattleState();
			break;
		case EPalMode::Worker:
			SwitchWorkerState();
			break;
		case EPalMode::Carrier:
			break;
	}
}

// Called to bind functionality to input
void APalYeti::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APalYeti::SwitchWildState()
{
	switch (PalWildState)
	{
		case EPalWildState::Patrol:
			HandleWildPatrol();
			break;
		case EPalWildState::PlayerHitToPal:
			HandleWildPlayerHitToPal();
			break;
		case EPalWildState::DetectPlayer:
			HandleWildDetectPlayer();
			break;
		case EPalWildState::Chase:
			HandleWildChase();
			break;
		case EPalWildState::Attack:
			HandleWildAttack();
			break;
		case EPalWildState::Escape:
			HandleWildEscape();
			break;
		case EPalWildState::Return:
			HandleWildReturn();
			break;
	}
}

void APalYeti::SwitchBattleState()
{
	switch (PalBattleState)
	{
		case EPalBattleState::FollowPlayer:
			HandleBattleFollowPlayer();
			break;
		case EPalBattleState::DetectTarget:
			HandleBattleDetectTarget();
			break;
		case EPalBattleState::Chase:
			HandleBattleChase();
			break;
		case EPalBattleState::Attack:
			HandleBattleAttack();
			break;
	}
}

void APalYeti::SwitchWorkerState()
{
	switch (PalWorkerState)
	{
		case EPalWorkerState::Idle:
			HandleWorkerIdle();
			break;
		case EPalWorkerState::FindWork:
			HandleWorkerFindWork();
			break;
		case EPalWorkerState::MoveToTarget:
			HandleWorkerMovetoTarget();
			break;
		case EPalWorkerState::Working:
			HandleWorkerWorking();
			break;
		case EPalWorkerState::Return:
			HandleWorkerReturn();
			break;
	}
}

void APalYeti::HandleWildPatrol()
{
	//UE_LOG(PalYeti, Warning, TEXT("[PalYeti, HandleWildPatrol] WildState : Patrol Started"));
	
	//일정 범위 안에서 랜덤하게 이동하면서 순찰하기
	//네비게이션 시스템 받아오기
	UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NaviSystem)
	{
		return;
	}

	APWAIController* MyController = Cast<APWAIController>(GetController());
	if (!MyController)
	{
		return;
	}
	if (!bIsPatroling)
	{
		//지정해둔 범위 내에 랜덤 위치 받아오기
		FNavLocation RandomPoint;
		//첫 시작 지점을 기준으로 지정
		InitLocation = this->GetActorLocation();
		bool bFound = NaviSystem->GetRandomReachablePointInRadius(InitLocation, PatrolRadius,RandomPoint);
		if (bFound)
		{
			//타겟 지정해서 저장, 애니메이션 실행, 팰 목표 장소 이동
			CurrentPatrolTargetLocation = RandomPoint.Location;
			bIsPatroling = true;
			YetiAnimInstance->bIsPatroling = this->bIsPatroling;
			this->GetCharacterMovement()->MaxWalkSpeed = 20.f;
			MyController->MoveToLocation(CurrentPatrolTargetLocation);
			
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol bIsPatroling = %d"), bIsPatroling);
		}
	}
	//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	{
		//애니메이션 변경 및 다음 목표 위치로 이동하도록
		bIsPatroling = false;
		YetiAnimInstance->bIsPatroling = this->bIsPatroling;
		//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWildPatrol] Patrol Reached TargetLocation"));
	}

	//UE_LOG(PalYeti, Warning, TEXT("[HandleWildPatrol] Player Distance = %f"), FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()));
	if (FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()) < PlayerDetectRadius)
	{
		//DetectPlayer 상태 변경
		SetPalWildState(EPalWildState::DetectPlayer);
	}
}

void APalYeti::HandleWildPlayerHitToPal()
{
}

void APalYeti::HandleWildDetectPlayer()
{
	UE_LOG(PalYeti, Warning, TEXT("[HandleWildDetectPlayer] WildState : DetectPlayer"));

	//Escape 상태 변경
	SetPalWildState(EPalWildState::Chase);
}

void APalYeti::HandleWildChase()
{
}

void APalYeti::HandleWildAttack()
{
}

void APalYeti::HandleWildEscape()
{
}

void APalYeti::HandleWildReturn()
{
}

void APalYeti::HandleBattleFollowPlayer()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleBattleFollowPlayer] BattleState : FollowPlayer"));
}

void APalYeti::HandleBattleDetectTarget()
{
}

void APalYeti::HandleBattleChase()
{
}

void APalYeti::HandleBattleAttack()
{
}

void APalYeti::HandleWorkerIdle()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerIdle] WorkerState : Idle Started"));
	
	//일정 범위 안에서 랜덤하게 이동하면서 순찰하기
	//네비게이션 시스템 받아오기
	UNavigationSystemV1* NaviSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NaviSystem)
	{
		return;
	}

	APWAIController* MyController = Cast<APWAIController>(GetController());
	if (!MyController)
	{
		return;
	}
	if (!bIsPatroling)
	{
		//지정해둔 범위 내에 랜덤 위치 받아오기
		FNavLocation RandomPoint;
		//첫 시작 지점을 기준으로 지정
		InitLocation = this->GetActorLocation();
		bool bFound = NaviSystem->GetRandomReachablePointInRadius(InitLocation, PatrolRadius,RandomPoint);
		if (bFound)
		{
			//타겟 지정해서 저장, 애니메이션 실행, 팰 목표 장소 이동
			CurrentPatrolTargetLocation = RandomPoint.Location;
			bIsPatroling = true;
			YetiAnimInstance->bIsPatroling = this->bIsPatroling;
			this->GetCharacterMovement()->MaxWalkSpeed = 20.f;
			MyController->MoveToLocation(CurrentPatrolTargetLocation);
			
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
			//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol bIsPatroling = %d"), bIsPatroling);
		}
	}
	//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	{
		//애니메이션 변경 및 다음 목표 위치로 이동하도록
		bIsPatroling = false;
		YetiAnimInstance->bIsPatroling = this->bIsPatroling;
		//UE_LOG(PalChicken, Warning, TEXT("[PalYeti, HandleWorkerIdle] Patrol Reached TargetLocation"));
	}
}

void APalYeti::HandleWorkerFindWork()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerFindWork] WorkerState : FindWork, WorkerPalName : %s"), *this->GetName());

	//타겟 자원이 있다면
	if (TargetResource)
	{
		bIsPatroling = false;
		YetiAnimInstance->bIsPatroling = this->bIsPatroling;
		SetPalWorkerState(EPalWorkerState::MoveToTarget, TargetResource);
	}
}

void APalYeti::HandleWorkerMovetoTarget()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerMovetoTarget] WorkerState : MovetoTarget, WorkerPalName : %s"), *this->GetName());

	//Target 자원으로 이동하기
	 FVector meLoc = this->GetActorLocation();
	 FVector targetLoc = TargetResource->GetActorLocation();
	// FVector dir = (targetLoc - meLoc).GetSafeNormal2D();
	//
	// //this->SetActorLocation(meLoc + dir * MoveSpeed * GetWorld()->GetDeltaSeconds());
	// AddMovementInput(dir);

	//AIController Move To 
	APWAIController* MyAIController = Cast<APWAIController>(GetController());
	if (MyAIController)
	{
		if (!bIsMoveToTarget)
		{
			if (TargetResource)
			{
				this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
				MyAIController->MoveToLocation(targetLoc);
				bIsMoveToTarget = true;
				YetiAnimInstance->bIsMove = bIsMoveToTarget;
			}
		}
	}

	//거리가 150보다 작으면 Working State 시작
	if (FVector::DistXY(meLoc, targetLoc) < 150.f)
	{
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		YetiAnimInstance->bIsMove = bIsMoveToTarget;

		//이동 정지 및 작업 상태 시작
		MyAIController->StopMovement();
		SetPalWorkerState(EPalWorkerState::Working, TargetResource);
		
		//작업중 애니메이션 시작
		bIsPlayingWorkAnim = true;
		YetiAnimInstance->bIsWorking = bIsPlayingWorkAnim;
	}
	
}

void APalYeti::HandleWorkerWorking()
{
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, HandleWorkerWorking] WorkerState : Working, WorkerPalName : %s"), *this->GetName());
	if (!TargetResource)
	{
		return;
	}
	//이미 타이머가 실행중이면 중복 실행 방지
	if (GetWorldTimerManager().IsTimerActive(WorkTimerHandle))
	{
		return;
	}
	//팰 작업속도
	if (ATree* tree = Cast<ATree>(TargetResource))
	{
		WorkSpeed = *YetiInfo.WorkSpeeds.Find("Tree");
	}
	else if (ARock* rock = Cast<ARock>(TargetResource))
	{
		WorkSpeed = *YetiInfo.WorkSpeeds.Find("Rock");
	}
	
	WorkInterval = 1.f / WorkSpeed;
	//WorkInterval마다 working 타이머 반복 실행
	GetWorldTimerManager().SetTimer(WorkTimerHandle, this, &APalYeti::PalWorking, WorkInterval, true);

	
}

void APalYeti::HandleWorkerReturn()
{
	//Idle 상태로 변경
	this->SetPalWorkerState(EPalWorkerState::Idle, nullptr);
}

void APalYeti::SetTableData()
{
	Super::SetTableData();

	//팰 속도
	MoveSpeed = YetiInfo.MoveSpeed;
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	
}

void APalYeti::PalWorking()
{
	Super::PalWorking();
	//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] WorkerState : Working, WorkerPalName : %s"), *this->GetName());

	//자원에 데미지 10씩 주기
	UGameplayStatics::ApplyDamage(TargetResource, 10.f, GetController(), this, nullptr);
	
	//데미지 주다가 자원이 비활성화 되면, 자원과 팰 둘다 작업상태 멈춤 Rest상태로 Rest 배열에 자동 추가됨(자원은 재생성될때)
	if (ATree* tree = Cast<ATree>(TargetResource))
	{
		//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] tree->IsActive() : %d, tree->IsBeingWorkedOn() : %d,  WorkerPalName : %s"), tree->IsActive(), tree->IsBeingWorkedOn(), *this->GetName());
		if (tree && !tree->IsActive())
		{
			//자원과 팰 둘다 작업상태 멈춤
			tree->SetIsBeingWorkedOn(false);
			this->SetPalIsWorking(false);

			//작업중 애니메이션 취소
			bIsPlayingWorkAnim = false;
			YetiAnimInstance->bIsWorking = false;

			//팰 상태 Return으로 초기화
			this->SetPalWorkerState(EPalWorkerState::Return, nullptr);
			
			//Work 배열에서 제거
			// 	PalBox->WorkedPalActors.Remove(this);
			// 	UE_LOG(PalLog, Warning, TEXT("[PalWorking] Worked Pal : %s WorkedResourceActors Removed"), *this->GetName());
			// }

		}
	}
	if (ARock* rock = Cast<ARock>(TargetResource))
	{
		//UE_LOG(PalLog, Warning, TEXT("[PalYeti, PalWorking] rock->IsActive() : %d, rock->IsBeingWorkedOn() : %d, WorkerPalName : %s"), rock->IsActive(), rock->IsBeingWorkedOn(), *this->GetName());
		if (rock && !rock->IsActive())
		{
			//자원과 팰 둘다 작업상태 멈춤
			rock->SetIsBeingWorkedOn(false);
			this->SetPalIsWorking(false);

			//작업중 애니메이션 취소
			bIsPlayingWorkAnim = false;
			YetiAnimInstance->bIsWorking = false;

			//팰 상태 Return으로 초기화
			this->SetPalWorkerState(EPalWorkerState::Return, nullptr);
			
			//Work 배열에서 제거
			// 	PalBox->WorkedPalActors.Remove(this);
			// 	UE_LOG(PalLog, Warning, TEXT("[PalWorking] Worked Pal : %s WorkedResourceActors Removed"), *this->GetName());
			// }
			
		}
	}
}

void APalYeti::SetPalMode(EPalMode Mode)
{
	Super::SetPalMode(Mode);

	if (Mode == EPalMode::Wild)
	{
		PalMode = EPalMode::Wild;
		
		bIsWildMode = true;
		bIsBattleMode = false;
		bIsWorkerMode = false;
	}
	else if (Mode == EPalMode::Battle)
	{
		PalMode = EPalMode::Battle;
		
		bIsWildMode = false;
		bIsBattleMode = true;
		bIsWorkerMode = false;
	}
	else if (Mode == EPalMode::Worker)
	{
		PalMode = EPalMode::Worker;
		
		bIsWildMode = false;
		bIsBattleMode = false;
		bIsWorkerMode = true;
	}
}

void APalYeti::SetPalWildState(EPalWildState State)
{
	Super::SetPalWildState(State);

	PalWildState = State;
}

void APalYeti::SetPalBattleState(EPalBattleState State)
{
	Super::SetPalBattleState(State);

	PalBattleState = State;
}

void APalYeti::SetPalWorkerState(EPalWorkerState State, AActor* TargetActor)
{
	Super::SetPalWorkerState(State, TargetActor);

	PalWorkerState = State;
	//Target이 있다면
	if (TargetActor)
	{
		TargetResource = TargetActor;
	}
}

bool APalYeti::GetPalIsWorking() const
{
	return Super::GetPalIsWorking();
}

void APalYeti::SetPalIsWorking(bool IsWorking)
{
	Super::SetPalIsWorking(IsWorking);

	bIsWorking = IsWorking;
}
