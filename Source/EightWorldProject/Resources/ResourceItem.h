// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "ResourceItem.generated.h"

/**
 * 자원 아이템 클래스
 * 자원(나무, 돌)에서 드롭되는 아이템
 */
UCLASS()
class EIGHTWORLDPROJECT_API AResourceItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AResourceItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// 아이템 초기화
	UFUNCTION(BlueprintCallable, Category = "ResourceItem")
	virtual void Initialize();

	// 아이템 습득
	UFUNCTION(BlueprintCallable, Category = "ResourceItem")
	void Pickup(AActor* Collector);

	// 오버랩 이벤트 처리
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// 리소스 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	FName ResourceID;

	// 아이템 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	int32 Quantity = 1;

	// 아이템 자동 습득 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	float PickupRadius = 50.0f;

	// 아이템 자동 소멸 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	float DespawnTime = 60.0f;

	// 스태틱 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	class UStaticMeshComponent* Mesh;

	// 충돌체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ResourceItem")
	class USphereComponent* SphereCollider;

public:
	//운반중 여부
	UPROPERTY(Replicated)
	bool bIsMove;
	//운반중 상태 Set
	void SetIsMove(bool state);

private:
	// 소멸 타이머
	float DespawnTimer = 0.0f;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
