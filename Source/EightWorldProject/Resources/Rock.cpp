

#include "Rock.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ResourceManager.h"
#include "ResourceDataManager.h"

ARock::ARock()
{
	PrimaryActorTick.bCanEverTick = true;

	// 메시 컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 충돌 컴포넌트 생성
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Mesh);
	Box->SetBoxExtent(FVector(100, 100, 100));
	
	// 충돌 설정
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	// 리소스 ID 설정
	ResourceID = TEXT("Rock");

	// 태그 추가
	Tags.Add(FName("Rock"));

	// 기본 체력 초기화
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	ItemSpawnHealthInterval = 30.0f;
	
	// ResourceDataManager에서 설정 값 가져오기 시도
	UResourceDataManager* DataManager = UResourceDataManager::GetInstance();
	if (DataManager)
	{
		FResourceData* ResourceData = DataManager->GetResourceData(ResourceID);
		if (ResourceData)
		{
			// 데이터 테이블에서 값 가져오기
			MaxHealth = ResourceData->MaxHealth;
			MinHealth = ResourceData->MinHealth;
			ItemSpawnHealthInterval = ResourceData->ItemSpawnHealthInterval;
			
			// 에셋 경로 데이터 기반 로드
			if (!ResourceData->ResourceMeshPath.IsEmpty())
			{
				UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *ResourceData->ResourceMeshPath));
				if (LoadedMesh)
				{
					Mesh->SetStaticMesh(LoadedMesh);
				}
			}
		}
	}
	
	// 기본 메시 에셋 로드 (데이터 관리자에서 가져오지 못한 경우)
	if (!Mesh->GetStaticMesh())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Rock/Rock.Rock'"));
		if (MeshAsset.Succeeded())
		{
			Mesh->SetStaticMesh(MeshAsset.Object);
		}
	}
}

void ARock::BeginPlay()
{
	Super::BeginPlay();
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 활성화 상태 설정
	bIsActive = true;
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 데미지 처리 함수 재정의
float ARock::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
	
	// 체력이 아이템 스폰 간격만큼 감소했는지 확인
	if (LastItemSpawnHealth - CurrentHealth >= ItemSpawnHealthInterval)
	{
		// 아이템 스폰
		SpawnResourceItem();
		
		// 마지막 아이템 스폰 체력 업데이트
		LastItemSpawnHealth = CurrentHealth;
	}
	
	// 체력이 최소 체력 이하로 감소했는지 확인
	if (CurrentHealth <= MinHealth)
	{
		// 비활성화
		Deactivate();
		
		// ResourceManager에 비활성화 알림
		AResourceManager* ResourceManager = AResourceManager::GetInstance();
		if (ResourceManager)
		{
			ResourceManager->DeactivateRock(this);
		}
	}
	
	return ActualDamage;
}

// 활성화
void ARock::Activate()
{
	// 활성화 상태 설정
	bIsActive = true;
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 콜리전 활성화
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	// 메시 표시
	Mesh->SetVisibility(true);
}

// 비활성화
void ARock::Deactivate()
{
	// 비활성화 상태 설정
	bIsActive = false;
	
	// 콜리전 비활성화
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 메시 숨김
	Mesh->SetVisibility(false);
}

// 아이템 스폰
void ARock::SpawnResourceItem()
{
	// ResourceManager를 통해 아이템 스폰
	AResourceManager* ResourceManager = AResourceManager::GetInstance();
	if (!ResourceManager)
	{
		return;
	}
	
	// ResourceDataManager를 통해 데이터 가져오기
	UResourceDataManager* DataManager = UResourceDataManager::GetInstance();
	if (DataManager)
	{
		// 자원 데이터 가져오기
		FResourceData* ResourceData = DataManager->GetResourceData(ResourceID);
		if (ResourceData)
		{
			// 아이템 스폰 위치 계산 (자원 주변에 랜덤하게 스폰)
			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.X += FMath::RandRange(-50.0f, 50.0f);
			SpawnLocation.Y += FMath::RandRange(-50.0f, 50.0f);
			SpawnLocation.Z += 50.0f; // 약간 위에 스폰
			
			// 리소스 ID와 동일한 아이템 ID를 가진 아이템 스폰
			FName ItemID = ResourceID; // 리소스 ID와 동일한 아이템 ID 사용
			
			// 아이템 데이터 확인
			FItemData* ItemData = DataManager->GetItemData(ItemID);
			if (ItemData)
			{
				// 아이템 스폰
				ResourceManager->SpawnResourceItem(ItemID, SpawnLocation, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Warning, TEXT("[Rock] 아이템 스폰: %s"), *ItemID.ToString());
				return;
			}
		}
	
		// 데이터가 없는 경우 기본 아이템 스폰
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.X += FMath::RandRange(-50.0f, 50.0f);
		SpawnLocation.Y += FMath::RandRange(-50.0f, 50.0f);
		SpawnLocation.Z += 50.0f; // 약간 위에 스폰
	
		// 기본 아이템 스폰
		ResourceManager->SpawnResourceItem(ResourceID, SpawnLocation, FRotator::ZeroRotator);
		UE_LOG(LogTemp, Warning, TEXT("[Rock] 기본 아이템 스폰: %s"), *ResourceID.ToString());
	}
}