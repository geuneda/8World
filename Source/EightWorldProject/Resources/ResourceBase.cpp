#include "ResourceBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ResourceManager.h"
#include "ResourceDataManager.h"
#include "../Public/PWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AResourceBase::AResourceBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 메시 컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 충돌 컴포넌트 생성
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Mesh);
	
	// 충돌 설정
	Box->SetCollisionProfileName(TEXT("Resource"));
	
	// 기본 체력 초기화
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	ItemSpawnHealthInterval = 30.0f;

	Mesh->SetCanEverAffectNavigation(false);
	
	bReplicates = true;
}

void AResourceBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 활성화 상태 설정
	bIsActive = true;
}

void AResourceBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AResourceBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 비활성화 상태면 데미지 무시
	if (!bIsActive)
	{
		return 0.0f;
	}
	
	// 부모 클래스의 TakeDamage 호출
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 체력 감소
	float PreviousHealth = CurrentHealth;
	CurrentHealth -= ActualDamage;
	
	// ResourceManager에 데미지 알림
	AResourceManager* ResourceManager = GetResourceManager();
	if (ResourceManager)
	{
		// 체력이 아이템 스폰 간격만큼 감소했는지 확인
		if (LastItemSpawnHealth - CurrentHealth >= ItemSpawnHealthInterval)
		{
			// 아이템 스폰 요청
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.X += FMath::RandRange(-50.0f, 50.0f);
			SpawnLocation.Y += FMath::RandRange(-50.0f, 50.0f);
			SpawnLocation.Z += 50.0f; // 약간 위에 스폰
			
			// ResourceManager를 통해 아이템 스폰
			ResourceManager->SpawnResourceItem(ResourceID, SpawnLocation, FRotator::ZeroRotator);
			
			// 마지막 아이템 스폰 체력 업데이트
			LastItemSpawnHealth = CurrentHealth;
		}
		
		// 체력이 최소 체력 이하로 감소했는지 확인
		if (CurrentHealth <= MinHealth)
		{
			// 자식 클래스에서 구현한 비활성화 처리 호출
			HandleDeactivation();
		}
	}
	
	return ActualDamage;
}

void AResourceBase::Activate()
{
	// 활성화 상태 설정
	bIsActive = true;
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 콜리전 활성화
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	
	// 메시 표시
	//Mesh->SetVisibility(true);
	MultiRPC_Activate();
}

void AResourceBase::MultiRPC_Activate_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AResourceBase MultiRPC_Activate"));
	// 메시 표시
	Mesh->SetVisibility(true);
}

void AResourceBase::Deactivate()
{
	// 비활성화 상태 설정
	bIsActive = false;
	
	// 콜리전 비활성화
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	// 메시 숨김
	//Mesh->SetVisibility(false);
	MultiRPC_Deactivate();
}

void AResourceBase::MultiRPC_Deactivate_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT(" AResourceBase MultiRPC_Deactivate"));
	// 메시 숨김
	Mesh->SetVisibility(false);
}

UResourceDataManager* AResourceBase::GetResourceDataManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	APWGameMode* GameMode = Cast<APWGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
	{
		return nullptr;
	}
	
	return GameMode->GetResourceDataManager();
}

AResourceManager* AResourceBase::GetResourceManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	APWGameMode* GameMode = Cast<APWGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
	{
		return nullptr;
	}
	
	return GameMode->GetResourceManager();
}

void AResourceBase::SetIsBeingWorkedOn(bool bInIsBeingWorkedOn)
{
	bIsBeingWorkedOn = bInIsBeingWorkedOn;
}

void AResourceBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AResourceBase, bIsActive);
	DOREPLIFETIME(AResourceBase, CurrentHealth);
	DOREPLIFETIME(AResourceBase, LastItemSpawnHealth);
}
