// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceDataManager.h"
#include "ResourceManager.generated.h"

class ARock;
class ATree;
class AResourceItem;
class UResourceDataManager;

/**
 * 자원 관리자 클래스
 * 자원 오브젝트(나무, 돌)의 풀링 및 관리를 담당
 */
UCLASS()
class EIGHTWORLDPROJECT_API AResourceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AResourceManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 돌 오브젝트 활성화
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	ARock* ActivateRock(const FVector& Location, const FRotator& Rotation);

	// 돌 오브젝트 비활성화
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	void DeactivateRock(ARock* Rock);

	// 나무 오브젝트 활성화
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	ATree* ActivateTree(const FVector& Location, const FRotator& Rotation);

	// 나무 오브젝트 비활성화
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	void DeactivateTree(ATree* Tree);

	// 아이템 스폰
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	AResourceItem* SpawnResourceItem(FName ResourceID, const FVector& Location, const FRotator& Rotation);

	// 풀에 Rock 추가
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	void AddRockToPool(ARock* Rock);

	// 풀에 Tree 추가
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	void AddTreeToPool(ATree* Tree);

	// Rock이 풀에 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	bool IsRockInPool(ARock* Rock) const;

	// Tree가 풀에 있는지 확인
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	bool IsTreeInPool(ATree* Tree) const;

	// 비활성화된 자원 재활성화 타이머 업데이트
	void UpdateRespawnTimers(float DeltaTime);

	// 싱글톤 인스턴스 가져오기
	UFUNCTION(BlueprintCallable, Category = "ResourceManager")
	static AResourceManager* GetInstance();

	// 돌 오브젝트 풀
	UPROPERTY()
	TArray<ARock*> RockPool;

	// 나무 오브젝트 풀
	UPROPERTY()
	TArray<ATree*> TreePool;
	
private:

	// 비활성화된 돌 오브젝트와 재활성화 시간
	UPROPERTY()
	TMap<ARock*, float> DeactivatedRocks;

	// 비활성화된 나무 오브젝트와 재활성화 시간
	UPROPERTY()
	TMap<ATree*, float> DeactivatedTrees;

	// 풀 초기 크기
	UPROPERTY(EditAnywhere, Category = "ResourceManager")
	int32 InitialPoolSize = 10;

	// 자원 재생성 시간
	UPROPERTY(EditAnywhere, Category = "ResourceManager")
	float RespawnTime = 60.0f; // 1분

	// 자원 데이터 관리자 참조
	UPROPERTY()
	UResourceDataManager* ResourceDataManager;
	
	// PWGameMode에서 ResourceDataManager 가져오기
	UResourceDataManager* GetResourceDataManager();

	// 자원 데이터 테이블 경로
	UPROPERTY(EditAnywhere, Category = "ResourceManager")
	FString ResourceDataTablePath = TEXT("/Game/Data/ResourceDataTable");
	
	// 아이템 데이터 테이블 경로
	UPROPERTY(EditAnywhere, Category = "ResourceManager")
	FString ItemDataTablePath = TEXT("/Game/Data/ItemDataTable");

	// 싱글톤 인스턴스
	static AResourceManager* Instance;
};
