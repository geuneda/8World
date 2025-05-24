// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rock.generated.h"

/**
 * 돌 자원 클래스
 * 플레이어가 공격하여 자원을 채집할 수 있는 오브젝트
 */
UCLASS()
class EIGHTWORLDPROJECT_API ARock : public AActor
{
	GENERATED_BODY()

public:
	ARock();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 데미지 처리 함수 재정의
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 활성화
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void Activate();

	// 비활성화
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void Deactivate();

	// 활성화 상태 확인
	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsActive() const { return bIsActive; }

	// 작업 중인지 확인
	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool IsBeingWorkedOn() const { return bIsBeingWorkedOn; }

	// 작업 중 상태 설정
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void SetIsBeingWorkedOn(bool bInIsBeingWorkedOn);

	// 리소스 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	FName ResourceID;

public:
	// 스태틱 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	class UStaticMeshComponent* Mesh;

	// 충돌체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	class UBoxComponent* Box;

	// 현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float CurrentHealth;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float MaxHealth = 100.0f;

	// 최소 체력 (이 체력 이하로는 파괴되지 않음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float MinHealth = 0.0f;

	// 아이템 스폰 체력 간격 (이 만큼 체력이 깎일 때마다 아이템 스폰)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
	float ItemSpawnHealthInterval = 30.0f;

private:
	// 활성화 상태
	bool bIsActive = true;

	// 작업 중인지 여부 (팰이 작업 중인지 확인용)
	bool bIsBeingWorkedOn = false;

	// 마지막 아이템 스폰 체력
	float LastItemSpawnHealth;
	
	// ResourceDataManager 가져오기
	class UResourceDataManager* GetResourceDataManager();
	
	// ResourceManager 가져오기
	class AResourceManager* GetResourceManager();
};
