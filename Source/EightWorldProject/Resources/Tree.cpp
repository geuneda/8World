
#include "Tree.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ResourceManager.h"
#include "ResourceDataManager.h"
#include "../Public/PWGameMode.h"
#include "Kismet/GameplayStatics.h"
ATree::ATree()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 메시 컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 충돌 컴포넌트 생성
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Mesh);
	Box->SetBoxExtent(FVector(50.0f, 50.0f, 150.0f));
	
	// 충돌 설정
	Box->SetCollisionProfileName(TEXT("Resource"));

	// 리소스 ID 설정
	ResourceID = TEXT("Tree");

	// 태그 추가
	Tags.Add(FName("Tree"));

	// 기본 체력 초기화
	MaxHealth = 100.0f;
	MinHealth = 0.0f;
	ItemSpawnHealthInterval = 30.0f;
	
	// ResourceDataManager에서 설정 값 가져오기 시도
	UResourceDataManager* DataManager = GetResourceDataManager();
	if (DataManager)
	{
		FResourceData* ResourceData = DataManager->GetResourceData(ResourceID);
		if (ResourceData)
		{
			// 데이터 테이블에서 값 가져오기
			MaxHealth = ResourceData->MaxHealth;
			MinHealth = ResourceData->MinHealth;
			ItemSpawnHealthInterval = ResourceData->ItemSpawnHealthInterval;
			
			// 메시 에셋 경로가 있는 경우 로드
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
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Tree/Tree.Tree'"));
		if (MeshAsset.Succeeded())
		{
			Mesh->SetStaticMesh(MeshAsset.Object);
		}
	}
}

// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 활성화 상태 설정
	bIsActive = true;
}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 데미지 처리 함수 재정의
float ATree::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
			// ResourceManager에 비활성화 요청
			ResourceManager->DeactivateTree(this);
		}
	}
	
	return ActualDamage;
}

// 활성화
void ATree::Activate()
{
	// 활성화 상태 설정
	bIsActive = true;
	
	// 체력 초기화
	CurrentHealth = MaxHealth;
	LastItemSpawnHealth = MaxHealth;
	
	// 콜리전 활성화
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	
	// 메시 표시
	Mesh->SetVisibility(true);
}

// 비활성화
void ATree::Deactivate()
{
	// 비활성화 상태 설정
	bIsActive = false;
	
	// 콜리전 비활성화
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	// 메시 숨김
	Mesh->SetVisibility(false);
}

// PWGameMode에서 ResourceDataManager 가져오기
UResourceDataManager* ATree::GetResourceDataManager()
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

// PWGameMode에서 ResourceManager 가져오기
AResourceManager* ATree::GetResourceManager()
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

// 작업 중 상태 설정
void ATree::SetIsBeingWorkedOn(bool bInIsBeingWorkedOn)
{
	bIsBeingWorkedOn = bInIsBeingWorkedOn;
}
