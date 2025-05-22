#include "PWGameMode.h"
#include "EightWorldProject/Resources/ResourceManager.h"
#include "EightWorldProject/Resources/Rock.h"
#include "EightWorldProject/Resources/Tree.h"
#include "Kismet/GameplayStatics.h"

APWGameMode::APWGameMode()
{
	ResourceManager = nullptr;
}

void APWGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// 리소스 매니저 생성
	CreateResourceManager();
}

void APWGameMode::CreateResourceManager()
{
	// 이미 생성되어 있는지 확인
	if (ResourceManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PWGameMode] ResourceManager가 이미 존재합니다."));
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[PWGameMode] World를 찾을 수 없습니다."));
		return;
	}
	
	// ResourceManager 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ResourceManager = World->SpawnActor<AResourceManager>(AResourceManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (!ResourceManager)
	{
		UE_LOG(LogTemp, Error, TEXT("[PWGameMode] ResourceManager 생성 실패"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[PWGameMode] ResourceManager 생성 성공"));
	
	// 월드에 존재하는 Rock과 Tree 액터 찾기
	TArray<AActor*> FoundRocks;
	TArray<AActor*> FoundTrees;
	
	UGameplayStatics::GetAllActorsOfClass(World, ARock::StaticClass(), FoundRocks);
	UGameplayStatics::GetAllActorsOfClass(World, ATree::StaticClass(), FoundTrees);
	
	UE_LOG(LogTemp, Warning, TEXT("[PWGameMode] 찾은 Rock 액터: %d개"), FoundRocks.Num());
	UE_LOG(LogTemp, Warning, TEXT("[PWGameMode] 찾은 Tree 액터: %d개"), FoundTrees.Num());
	
	// ResourceManager에 추가
	for (AActor* Actor : FoundRocks)
	{
		ARock* Rock = Cast<ARock>(Actor);
		if (Rock)
		{
			// 이미 풀에 있는 Rock은 추가하지 않음
			if (!ResourceManager->IsRockInPool(Rock))
			{
				ResourceManager->AddRockToPool(Rock);
			}
		}
	}
	
	for (AActor* Actor : FoundTrees)
	{
		ATree* Tree = Cast<ATree>(Actor);
		if (Tree)
		{
			// 이미 풀에 있는 Tree는 추가하지 않음
			if (!ResourceManager->IsTreeInPool(Tree))
			{
				ResourceManager->AddTreeToPool(Tree);
			}
		}
	}
}
