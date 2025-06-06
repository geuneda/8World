
#include "ResourceManager.h"
#include "Rock.h"
#include "Tree.h"
#include "ResourceItem.h"
#include "RockItem.h"
#include "TreeItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Public/PWGameMode.h"

AResourceManager* AResourceManager::Instance = nullptr;

AResourceManager::AResourceManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AResourceManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 싱글톤 인스턴스 설정
	Instance = this;
	
	// 리소스 데이터 관리자 가져오기
	ResourceDataManager = GetResourceDataManager();
	
	UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] BeginPlay: ResourceDataManager %s"), 
		ResourceDataManager ? TEXT("가져오기 성공") : TEXT("가져오기 실패"));
}

void AResourceManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 비활성화된 자원 재활성화 타이머 업데이트
	UpdateRespawnTimers(DeltaTime);
}

// 돌 오브젝트 활성화
ARock* AResourceManager::ActivateRock(const FVector& Location, const FRotator& Rotation)
{
	// 풀에서 비활성화된 돌 오브젝트 찾기
	for (ARock* Rock : RockPool)
	{
		if (!Rock->IsActive())
		{
			// 위치 및 회전 설정
			Rock->SetActorLocation(Location);
			Rock->SetActorRotation(Rotation);
			
			// 활성화
			Rock->Activate();
			
			// 비활성화 목록에서 제거
			DeactivatedRocks.Remove(Rock);
			
			return Rock;
		}
	}

	return nullptr;
}

// 돌 오브젝트 비활성화
void AResourceManager::DeactivateRock(ARock* Rock)
{
	if (Rock && Rock->IsActive())
	{
		// 비활성화 상태 설정
		Rock->Deactivate();
		
		// 재활성화 타이머 설정
		DeactivatedRocks.Add(Rock, RespawnTime);
		
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Rock 비활성화: %s"), *Rock->GetName());
	}
}

// 나무 오브젝트 활성화
ATree* AResourceManager::ActivateTree(const FVector& Location, const FRotator& Rotation)
{
	// 풀에서 비활성화된 나무 오브젝트 찾기
	for (ATree* Tree : TreePool)
	{
		if (!Tree->IsActive())
		{
			// 위치 및 회전 설정
			Tree->SetActorLocation(Location);
			Tree->SetActorRotation(Rotation);
			
			// 활성화
			Tree->Activate();
			
			// 비활성화 목록에서 제거
			DeactivatedTrees.Remove(Tree);
			
			return Tree;
		}
	}
	
	return nullptr;
}

// 나무 오브젝트 비활성화
void AResourceManager::DeactivateTree(ATree* Tree)
{
	if (Tree && Tree->IsActive())
	{
		// 비활성화 상태 설정
		Tree->Deactivate();
		
		// 재활성화 타이머 설정
		DeactivatedTrees.Add(Tree, RespawnTime);
		
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Tree 비활성화: %s"), *Tree->GetName());
	}
}

// 아이템 스폰
AResourceItem* AResourceManager::SpawnResourceItem(FName ResourceID, const FVector& Location, const FRotator& Rotation)
{
	// 리소스 데이터 관리자가 없으면 가져오기 시도
	if (!ResourceDataManager)
	{
		ResourceDataManager = GetResourceDataManager();
		if (!ResourceDataManager)
		{
			UE_LOG(LogTemp, Error, TEXT("[ResourceManager] SpawnResourceItem: 리소스 데이터 관리자 가져오기 실패"));
			return nullptr;
		}
	}
	
	// 리소스 데이터 테이블에서 아이템 정보 가져오기
	FItemData* ItemData = ResourceDataManager->GetItemData(ResourceID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] SpawnResourceItem: %s 아이템 데이터 없음"), *ResourceID.ToString());
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	// 아이템 생성 - 리소스 타입에 따라 적절한 클래스 생성
	AResourceItem* Item = nullptr;
	
	// 리소스 ID에 따라 적절한 아이템 클래스 생성
	if (ResourceID == TEXT("Rock"))
	{
		// 돌 아이템 생성
		Item = World->SpawnActor<ARockItem>(ARockItem::StaticClass(), Location, Rotation);
		Item->SetReplicates(true);
		Item->SetReplicateMovement(true);
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] 돌 아이템 생성"));
	}
	else if (ResourceID == TEXT("Tree"))
	{
		// 나무 아이템 생성
		Item = World->SpawnActor<ATreeItem>(ATreeItem::StaticClass(), Location, Rotation);
		Item->SetReplicates(true);
		Item->SetReplicateMovement(true);
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] 나무 아이템 생성"));
	}
	else
	{
		// 기본 아이템 생성
		Item = World->SpawnActor<AResourceItem>(AResourceItem::StaticClass(), Location, Rotation);
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] 기본 아이템 생성: %s"), *ResourceID.ToString());
	}
	
	if (Item)
	{
		// 아이템 ID 설정
		Item->ResourceID = ResourceID;
		
		// 아이템 데이터가 있는 경우 추가 정보 설정
		if (ItemData)
		{
			Item->Quantity = UKismetMathLibrary::RandomIntegerInRange(ItemData->MinDropQuantity, ItemData->MaxDropQuantity);
			UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] 아이템 스폰: %s, 수량: %d"), *ResourceID.ToString(), Item->Quantity);
		}
		
		// 아이템 초기화
		Item->Initialize();
	}
	
	return Item;
}

// 비활성화된 자원 재활성화 타이머 업데이트
void AResourceManager::UpdateRespawnTimers(float DeltaTime)
{
	// 비활성화된 돌 오브젝트 타이머 업데이트
	TArray<ARock*> RocksToRespawn;
	
	for (auto& Pair : DeactivatedRocks)
	{
		Pair.Value -= DeltaTime;
		
		if (Pair.Value <= 0.0f)
		{
			RocksToRespawn.Add(Pair.Key);
		}
	}
	
	// 재활성화 시간이 된 돌 오브젝트 활성화
	for (ARock* Rock : RocksToRespawn)
	{
		// 월드에서 랜덤 위치 선택 (추후 구현)
		FVector RandomLocation = Rock->GetActorLocation(); // 현재는 원래 위치에 재활성화
		
		ActivateRock(RandomLocation, Rock->GetActorRotation());
	}
	
	// 비활성화된 나무 오브젝트 타이머 업데이트
	TArray<ATree*> TreesToRespawn;
	
	for (auto& Pair : DeactivatedTrees)
	{
		Pair.Value -= DeltaTime;
		
		if (Pair.Value <= 0.0f)
		{
			TreesToRespawn.Add(Pair.Key);
		}
	}
	
	// 재활성화 시간이 된 나무 오브젝트 활성화
	for (ATree* Tree : TreesToRespawn)
	{
		// 월드에서 랜덤 위치 선택 (추후 구현)
		FVector RandomLocation = Tree->GetActorLocation(); // 현재는 원래 위치에 재활성화
		
		ActivateTree(RandomLocation, Tree->GetActorRotation());
	}
}

// 싱글톤 인스턴스 가져오기
AResourceManager* AResourceManager::GetInstance()
{
	if (!Instance)
	{
		// 월드에서 ResourceManager 찾기
		UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport, EGetWorldErrorMode::ReturnNull);
		
		if (World)
		{
			Instance = Cast<AResourceManager>(UGameplayStatics::GetActorOfClass(World, AResourceManager::StaticClass()));
			
			// 인스턴스가 없으면 생성
			if (!Instance)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				Instance = World->SpawnActor<AResourceManager>(AResourceManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
	
	return Instance;
}

// 풀에 Rock 추가
void AResourceManager::AddRockToPool(ARock* Rock)
{
	if (!Rock)
	{
		return;
	}
	
	// 이미 풀에 있는지 확인
	if (IsRockInPool(Rock))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Rock이 이미 풀에 있습니다."));
		return;
	}
	
	// 풀에 추가
	RockPool.Add(Rock);
	UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Rock을 풀에 추가했습니다. 현재 풀 크기: %d"), RockPool.Num());
}

// 풀에 Tree 추가
void AResourceManager::AddTreeToPool(ATree* Tree)
{
	if (!Tree)
	{
		return;
	}
	
	// 이미 풀에 있는지 확인
	if (IsTreeInPool(Tree))
	{
		UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Tree가 이미 풀에 있습니다."));
		return;
	}
	
	// 풀에 추가
	TreePool.Add(Tree);
	UE_LOG(LogTemp, Warning, TEXT("[ResourceManager] Tree를 풀에 추가했습니다. 현재 풀 크기: %d"), TreePool.Num());
}

// Rock이 풀에 있는지 확인
bool AResourceManager::IsRockInPool(ARock* Rock) const
{
	if (!Rock)
	{
		return false;
	}
	
	return RockPool.Contains(Rock);
}

// Tree가 풀에 있는지 확인
bool AResourceManager::IsTreeInPool(ATree* Tree) const
{
	if (!Tree)
	{
		return false;
	}
	
	return TreePool.Contains(Tree);
}

// PWGameMode에서 ResourceDataManager 가져오기
UResourceDataManager* AResourceManager::GetResourceDataManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[ResourceManager] GetResourceDataManager: World를 찾을 수 없습니다."));
		return nullptr;
	}
	
	APWGameMode* GameMode = Cast<APWGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("[ResourceManager] GetResourceDataManager: PWGameMode를 찾을 수 없습니다."));
		return nullptr;
	}
	
	UResourceDataManager* DataManager = GameMode->GetResourceDataManager();
	if (!DataManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[ResourceManager] GetResourceDataManager: ResourceDataManager가 없습니다."));
		return nullptr;
	}
	
	return DataManager;
}
