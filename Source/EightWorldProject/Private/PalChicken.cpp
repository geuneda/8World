// Fill out your copyright notice in the Description page of Project Settings.


#include "PalChicken.h"

#include "CommonStorageBox.h"
#include "NavigationSystem.h"
#include "PalBox.h"
#include "PalChickenAnimInstance.h"
#include "PWAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/Resources/ResourceItem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

DECLARE_LOG_CATEGORY_EXTERN(PalChicken, Log, All);
DEFINE_LOG_CATEGORY(PalChicken);

APalChicken::APalChicken()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Chicken Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Chiken/Chiken.Chiken'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90.f), FRotator(0,-90,0));
	}

	//Chicken Anim Class
	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnimInstance(TEXT("/Script/Engine.AnimBlueprint'/Game/PalWorld/Blueprints/ChickenAnimation/ABP_PalChicken.ABP_PalChicken_C'"));
	if (tempAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnimInstance.Class);
	}

	//DataTable RowName 초기화
	PalDataRowName = TEXT("Chicken");

	//팰 운반용 충돌체 초기화
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(GetMesh());
	SphereComp->SetSphereRadius(50.f);
	SphereComp->SetRelativeLocation(FVector(0, 50.f, 26.f));
}

void APalChicken::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//DataTable 로딩
		if (PalDataTable)
		{
			FPalInfoData* InfoData = PalDataTable->FindRow<FPalInfoData>(PalDataRowName, TEXT("Chicken"));
			if (InfoData)
			{
				ChickenInfo = *InfoData;
			}
		}

		//팰 DataTable Data 초기화
		GetWorldTimerManager().SetTimer(TableDataTimerHandle, this, &APalChicken::SetTableData, 0.1f, false);
	}


	//플레이어 소유 여부 (임시)
	bIsPlayerOwned = true;
	
	//팰 Carrier모드 시작 (임시)
	SetPalMode(EPalMode::Carrier);

	//팰 모드별 상태 초기화
	SetPalWildState(EPalWildState::Patrol);
	SetPalBattleState(EPalBattleState::FollowPlayer);
	SetPalCarrierState(EPalCarrierState::Patrol, nullptr);

	//팰 운반중 여부
	bIsCarrying = false;

	//팰 회전
	this->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	this->bUseControllerRotationYaw = false;
	this->GetCharacterMovement()->bOrientRotationToMovement = true;

	//애니메이션
	ChickenAnimInstance = Cast<UPalChickenAnimInstance>(GetMesh()->GetAnimInstance());
	
	//공용 저장 박스
	CommonStorageBox = Cast<ACommonStorageBox>(UGameplayStatics::GetActorOfClass(GetWorld(), ACommonStorageBox::StaticClass()));

	MyAIController = Cast<APWAIController>(GetController());

	//팰 박스
	palBox = Cast<APalBox>(UGameplayStatics::GetActorOfClass(GetWorld(), APalBox::StaticClass()));

	IntervalDamage = 10.f;
}

void APalChicken::Tick(float DeltaTime)
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
	case EPalMode::Carrier:
		SwitchCarrierState();
		break;
	case EPalMode::Worker:
		break;
	}
}

void APalChicken::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APalChicken::SwitchWildState()
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
	case EPalWildState::Die:
		HandleWildDie();
		break;
	}
}

void APalChicken::SwitchBattleState()
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
	case EPalBattleState::Die:
		HandleBattleDie();
		break;
	}
}

void APalChicken::SwitchCarrierState()
{
	switch (PalCarrierState)
	{
	case EPalCarrierState::Patrol:
		HandleCarrierPatrol();
		break;
	case EPalCarrierState::FindItem:
		HandleCarrierFindItem();
		break;
	case EPalCarrierState::MoveToTarget:
		HandleCarrierMovetoTarget();
		break;
	case EPalCarrierState::Carrying:
		HandleCarrierCarrying();
		break;
	case EPalCarrierState::Return:
		HandleCarrierReturn();
		break;
	}
}

void APalChicken::HandleWildPatrol()
{
	//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Patrol Started"));
	
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
			ChickenAnimInstance->bIsPatroling = this->bIsPatroling;
			this->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
			MyController->MoveToLocation(CurrentPatrolTargetLocation);
			
			//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
			//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Patrol bIsPatroling = %d"), bIsPatroling);
		}
	}
	//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	{
		//애니메이션 변경 및 다음 목표 위치로 이동하도록
		bIsPatroling = false;
		ChickenAnimInstance->bIsPatroling = this->bIsPatroling;
		//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Patrol Reached TargetLocation"));
	}

	//UE_LOG(PalChicken, Warning, TEXT("[HandleWildPatrol] Player Distance = %f"), FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()));
	if (player)
	{
		if (FVector::DistXY(this->GetActorLocation(), player->GetActorLocation()) < PlayerDetectRadius)
		{
			//DetectPlayer 상태 변경
			SetPalWildState(EPalWildState::DetectPlayer);
		}
	}
}

void APalChicken::HandleWildPlayerHitToPal()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleWildPlayerHitToPal] pal Name : %s,  WildState : PlayerHitToPal Started"), *this->GetName());

	APWAIController* MyController = Cast<APWAIController>(GetController());
	//공격 중인 애니메이션 취소
	if (bIsMoveToTarget)
	{
		bIsMoveToTarget = false;
		ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
	}
	//데미지 10씩 받을 때마다 데미지 애니메이션 실행
	if (!this->bIsDamaged)
	{
		this->bIsDamaged = true;
		ChickenAnimInstance->bIsDamaged = this->bIsDamaged;
	}

	MyController->StopMovement();
	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
}

void APalChicken::HandleWildDetectPlayer()
{
	//UE_LOG(PalChicken, Warning, TEXT("[HandleWildDetectPlayer] WildState : DetectPlayer"));

	//Escape 상태 변경
	SetPalWildState(EPalWildState::Escape);
}

void APalChicken::HandleWildChase()
{
}

void APalChicken::HandleWildAttack()
{
}

void APalChicken::HandleWildEscape()
{
	//실행중인 타이머 체크
	if (GetWorldTimerManager().IsTimerActive(EscapeTimerHandle))
	{
		return;
	}
	
	//patrol 애니메이션 취소
	if (bIsPatroling)
	{
		bIsPatroling = false;
		ChickenAnimInstance->bIsPatroling = bIsPatroling;
	}
	//이동 애니메이션 실행
	if (!bIsMoveToTarget)
	{
		bIsMoveToTarget = true;
		ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
	}
	
	//도망 함수 타이머
	GetWorldTimerManager().SetTimer(EscapeTimerHandle, this, &APalChicken::UpdateEscapeLocation, 0.2f, true);
}

void APalChicken::UpdateEscapeLocation()
{
	
	FVector playerLoc = player->GetActorLocation();
	FVector meLoc = this->GetActorLocation();
	APWAIController* MyController = Cast<APWAIController>(GetController());

	//플레이어 반대방향
	FVector RunDir = (meLoc - playerLoc).GetSafeNormal();
	
	//플레이어가 쫓아오고 있을 때 약간 좌/우로 틀기
	float RandomAngle = FMath::FRandRange(-45.0f, 45.0f);
	FVector TargetDir = RunDir.RotateAngleAxis(RandomAngle, FVector::UpVector);
	FRotator NewRotator = FMath::RInterpTo(RunDir.Rotation(), TargetDir.Rotation(), GetWorld()->GetDeltaSeconds(), 1.f);
	RunDir = NewRotator.Vector();
	
	//목표 도망 위치
	FVector TargetLoc = meLoc + RunDir * 700.f;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLoc;
	//목표 위치로 이동
	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(TargetLoc, 300.f, NavLoc))
	{
		this->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		MyController->MoveToLocation(NavLoc.Location);
	}
	//플레이어 감지 범위를 넘어서면 patrol 상태 전환
	if (FVector::DistXY(playerLoc, meLoc) > PlayerDetectDistance && CurHP > 0)
	{
		GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
		if(bIsMoveToTarget)
		{
			bIsMoveToTarget = false;
			ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
		}
		SetPalWildState(EPalWildState::Patrol);
	}
	if (CurHP <= 0)
	{
		SetPalWildState(EPalWildState::Die);
	}
}

void APalChicken::HandleWildDie()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleWildDie] WorkerState : Die, Pal Name : %s"), *this->GetName());
	GetWorldTimerManager().ClearTimer(EscapeTimerHandle);
	APWAIController* MyController = Cast<APWAIController>(GetController());
	if (MyController)
	{
		MyController->StopMovement();
	}
	if(bIsMoveToTarget)
	{
		bIsMoveToTarget = false;
		ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
	}

	//RagDoll
	//물리 시뮬레이션 활성화
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);


	//캡슐 콜라이더 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//컨트롤러 제거
	DetachFromControllerPendingDestroy();
	
	// //튕겨나가는 힘 추가
	if (LaunchImpulse == FVector::ZeroVector)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Launch Impulse"));
		LaunchImpulse = GetActorForwardVector() * 250.f + FVector(0, 0, 500.f);
		GetMesh()->AddImpulseToAllBodiesBelow(LaunchImpulse, NAME_None, true);
	}
		
}

float APalChicken::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	//작업, 운반 팰은 데미지 0
	if (this->bIsWorkerMode || this->bIsCarrierMode)
	{
		return 0.0f;
	}
	
	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//팰 체력 감소
	CurHP = FMath::Clamp(CurHP - damage, 0, MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("this pal Name = %s, CurHP = %f"), *this->GetName(), CurHP);
	
	//데미지 받았을 때 상태 전환
	if (this->bIsWildMode)
	{
		//데미지 IntervalDamage(10)씩 받을때마다 PlayerHitToPal로 변환
		if (LastHP - CurHP >= IntervalDamage)
		{
			LastHP = CurHP;
			SetPalWildState(EPalWildState::PlayerHitToPal);
		}
		//체력이 없으면 사망 상태 전환
		if (CurHP <= 0)
		{
			SetPalWildState(EPalWildState::Die);
		}

	}
	
	return damage;
}

void APalChicken::HandleBattleFollowPlayer()
{
}

void APalChicken::HandleBattleDetectTarget()
{
}

void APalChicken::HandleBattleChase()
{
}

void APalChicken::HandleBattleAttack()
{
}

void APalChicken::HandleBattleDie()
{
}

void APalChicken::HandleCarrierPatrol()
{
	//UE_LOG(PalChicken, Warning, TEXT("[HandleCarrierPatrol] Patrol Started"));
	
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
			ChickenAnimInstance->bIsPatroling = this->bIsPatroling;
			this->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
			MyController->MoveToLocation(CurrentPatrolTargetLocation);
			
			//UE_LOG(PalChicken, Warning, TEXT("[HandleCarrierPatrol] Patrol My MaxWalkSpeed = %f"), this->GetCharacterMovement()->MaxWalkSpeed);
			//UE_LOG(PalChicken, Warning, TEXT("[HandleCarrierPatrol] Patrol bIsPatroling = %d"), bIsPatroling);
		}
	}
	//UE_LOG(PalChicken, Warning, TEXT("[HandleCarrierPatrol] Patrol Distance = %f"), FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation));
	//목표 범위안에 들어가면 다시 새로운 지점으로 이동하게 하기 - Log에 40~41쯤 찍히면 타겟에 도착함
	if(FVector::DistXY(this->GetActorLocation(), CurrentPatrolTargetLocation) < 45.f)
	{
		//애니메이션 변경 및 다음 목표 위치로 이동하도록
		bIsPatroling = false;
		ChickenAnimInstance->bIsPatroling = this->bIsPatroling;
		//UE_LOG(PalChicken, Warning, TEXT("[HandleCarrierPatrol] Patrol Reached TargetLocation"));
	}
	
}

void APalChicken::HandleCarrierFindItem()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierFindItem] CarrierState : FindItem, CarrierPalName : %s"), *this->GetName());
	//타겟 아이템이 있다면
	if (TargetItem)
	{
		APWAIController* MyController = Cast<APWAIController>(GetController());
		MyController->StopMovement();
		//애니메이션 변경
		bIsPatroling = false;
		ChickenAnimInstance->bIsPatroling = this->bIsPatroling;

		//아이템이 굴러다닐 경우 때문에 1초뒤에 아이템 있는 곳으로 이동하도록 설정
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &APalChicken::SetCarrierMoveToTarget, 1.5f, false);
		//SetPalCarrierState(EPalCarrierState::MoveToTarget, TargetItem);
	}
}

void APalChicken::SetCarrierMoveToTarget()
{
	SetPalCarrierState(EPalCarrierState::MoveToTarget, TargetItem);
}

void APalChicken::HandleCarrierMovetoTarget()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierMovetoTarget] CarrierState : MovetoTarget, CarrierPalName : %s"), *this->GetName());

	//Target 자원으로 이동하기
	FVector meLoc = this->GetActorLocation();
	//AIController Move To 
	if (MyAIController)
	{
		if (!bIsMoveToTarget)
		{
			if (TargetItem)
			{
				//이동 및 이동 애니메이션 실행
				MyAIController->MoveToLocation(TargetItem->GetActorLocation());
				bIsMoveToTarget = true;
				ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
				//이동 속도 변경
				WorkSpeed = *ChickenInfo.WorkSpeeds.Find("Item");
				this->GetCharacterMovement()->MaxWalkSpeed = WorkSpeed;
			}
		}
	}

	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierMovetoTarget] CarrierState : MovetoTarget, Distance : %f"), FVector::DistXY(meLoc, targetLoc));
	//거리가 60보다 작으면 Carring State 시작
	AResourceItem* item = Cast<AResourceItem>(TargetItem);
	if (!TargetItem->IsActorBeingDestroyed())
	{
		if (FVector::DistXY(meLoc, TargetItem->GetActorLocation()) < 60.f)
		{
			//이동중 애니메이션 취소
			bIsMoveToTarget = false;
			ChickenAnimInstance->bIsMoving = bIsMoveToTarget;

			//이동 정지 및 작업 상태 시작
			MyAIController->StopMovement();
			SetPalCarrierState(EPalCarrierState::Carrying, TargetItem);
		
			//작업중 애니메이션 시작
			//bIsPlayingWorkAnim = true;
			//ChickenAnimInstance->bIsWorking = bIsPlayingWorkAnim;
		}
	}
	else if (TargetItem->IsActorBeingDestroyed()) // TargetItem Destroyed by Player
	{
		MyAIController->StopMovement();
		
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		ChickenAnimInstance->bIsMoving = bIsMoveToTarget;

		SetPalCarrierState(EPalCarrierState::Return, nullptr);
	}
	
}

void APalChicken::HandleCarrierCarrying()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierCarrying] CarrierState : Carrying, CarrierPalName : %s"), *this->GetName());
	FVector meLoc = this->GetActorLocation();
	TArray<AActor*> ItemActors;
	SphereComp->GetOverlappingActors(ItemActors, AResourceItem::StaticClass());
	if (ItemActors.Num() > 0)
	{
		for (AActor* item : ItemActors)
		{
			//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierCarrying] Item Name : %s"), *item->GetName());
			//아이템 상태 - 운반중 변경
			AResourceItem* itemActor = Cast<AResourceItem>(item);
			if (itemActor && itemActor == Cast<AResourceItem>(TargetItem)) // 범위안에 들어온 아이템들 중에 타겟 아이템과 같다면
			{
				itemActor->SetIsMove(true);
				//운반용 팰에 부착
				itemActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("CarrySocket"));
				//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierCarrying] CarrierState : Carrying, Item Actor : %s Attached To Pal Carrier"), *item->GetName());
				break;
			}
		}
		if (CommonStorageBox)
		{
			//공용 저장 박스로 이동하기
			if (MyAIController)
			{
				if (!bIsMoveToTarget)
				{
					//이동 및 이동 애니메이션 실행
					MyAIController->MoveToLocation(CommonStorageBox->GetActorLocation());
					bIsMoveToTarget = true;
					ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
				}
			}
		}
	}
	
	// if (TargetItem->IsActorBeingDestroyed()) // 플레이어의 의해서 아이템이 사라진 경우
	// {
	// 	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierCarrying] CarrierState : Carrying, TargetItem IsActorBeingDestroyed"));
	// 	//다시 처음으로 상태 변경
	// 	MyAIController->StopMovement();
	// 	
	// 	//이동중 애니메이션 취소
	// 	bIsMoveToTarget = false;
	// 	ChickenAnimInstance->bIsMoving = bIsMoveToTarget;
	// 	
	// 	SetPalCarrierState(EPalCarrierState::Return, nullptr);
	// }
	
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierCarrying] CarrierState : Carrying, Distance : %f"), FVector::DistXY(meLoc, CommonStorageBox->GetActorLocation()));
	//거리가 100 미만일때, 움직임을 멈추고 공용 창고 앞에서 운반 아이템 파괴(인벤토리 추가 기능 나중)
	if (FVector::DistXY(meLoc, CommonStorageBox->GetActorLocation()) < 100.f)
	{
		//이동중 애니메이션 취소
		bIsMoveToTarget = false;
		ChickenAnimInstance->bIsMoving = bIsMoveToTarget;

		//이동 정지 및 작업 상태 시작
		MyAIController->StopMovement();
		//아이템 파괴 타이머
		FTimerHandle DestroyTimerHandle;
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &APalChicken::CarriedItemDestroy, 0.5f, false);
	}
	
}

void APalChicken::CarriedItemDestroy()
{
	if (TargetItem)
	{
		//아이템 물리, 충돌체 등 다시 켜기
		Cast<AResourceItem>(TargetItem)->SetIsMove(false);
		//아이템 파괴
		TargetItem->Destroy();
		
		//반복을 위한 상태 변화
		SetPalCarrierState(EPalCarrierState::Return, nullptr);
	}
}

void APalChicken::HandleCarrierReturn()
{
	//UE_LOG(PalChicken, Warning, TEXT("[PalChicken, HandleCarrierReturn] CarrierState : Return, CarrierPalName : %s"), *this->GetName());
	//carrying 상태 비활성화, 쉬고 있는 팰에 저장
	this->SetPalIsCarrying(false);
	SetPalCarrierState(EPalCarrierState::Patrol, nullptr);
}

inline void APalChicken::SetTableData()
{
	Super::SetTableData();

	//팰 속도
	MoveSpeed = ChickenInfo.MoveSpeed;
	this->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	//팰 순찰속도
	PatrolSpeed = ChickenInfo.PatrolSpeed;
	
	//팰 도망속도
	RunSpeed = ChickenInfo.RunSpeed;
	
	//팰 체력
	MaxHP = ChickenInfo.HP;
	CurHP = MaxHP;
	LastHP = MaxHP;
}


void APalChicken::SetPalMode(EPalMode Mode)
{
	Super::SetPalMode(Mode);

	if (Mode == EPalMode::Wild)
	{
		PalMode = EPalMode::Wild;
		
		bIsWildMode = true;
		bIsBattleMode = false;
		bIsCarrierMode = false;
	}
	else if (Mode == EPalMode::Battle)
	{
		PalMode = EPalMode::Battle;
		
		bIsWildMode = false;
		bIsBattleMode = true;
		bIsCarrierMode = false;
	}
	else if (Mode == EPalMode::Carrier)
	{
		PalMode = EPalMode::Carrier;
		
		bIsWildMode = false;
		bIsBattleMode = false;
		bIsCarrierMode = true;
	}
}

void APalChicken::SetPalWildState(EPalWildState State)
{
	Super::SetPalWildState(State);

	PalWildState = State;
}

void APalChicken::SetPalBattleState(EPalBattleState State)
{
	Super::SetPalBattleState(State);
	
	PalBattleState = State;
}

void APalChicken::SetPalCarrierState(EPalCarrierState State, AActor* TargetActor)
{
	Super::SetPalCarrierState(State, TargetActor);

	PalCarrierState = State;
	//Target이 있다면
	if (TargetActor)
	{
		TargetItem = TargetActor;
	}
}

bool APalChicken::GetPalIsCarrying() const
{
	return Super::GetPalIsCarrying();
}

void APalChicken::SetPalIsCarrying(bool IsCarrying)
{
	Super::SetPalIsCarrying(IsCarrying);

	bIsCarrying = IsCarrying;
}

void APalChicken::CaptureByPlayer()
{
	//팰이 사라지면 전체 팰 배열에서 제거
	if (palBox->AllPalsInMap.Contains(this))
	{
		palBox->AllPalsInMap.Remove(this);
		UE_LOG(PalChicken, Warning, TEXT("[CaptureByPlayer]Removed AllPalsInMap Actor : %s"), *this->GetName());
	}
	
	Super::CaptureByPlayer();
}
