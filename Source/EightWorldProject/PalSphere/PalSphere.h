// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PalSphere.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APalSphere : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APalSphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 충돌 처리 함수
	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, const FHitResult& Hit);

	// 팰스피어 발사 함수
	void FireInDirection(const FVector& Direction);

	// 팰스피어 물리 시뮬레이션 활성화
	void EnablePhysicsSimulation();

	// 팰스피어 자동 파괴 타이머 설정
	void SetDestroyTimer();

public:
	// 팰 스피어 피격 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class USphereComponent* SphereComp;

	// 팰 스피어 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class USkeletalMeshComponent* MeshComp;

	// 팰 스피어 발사체 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class UProjectileMovementComponent* ProjectileMovement;

	// 팰 스피어 발사 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	float InitialSpeed = 1500.0f;

	// 팰 스피어 최대 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	float MaxSpeed = 2000.0f;

	// 팰 스피어 캡처 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class UNiagaraSystem* CaptureEffect;

	// 팰 스피어 자동 파괴 시간(초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	float DestroyTime = 3.0f;

	// 타이머 핸들
	FTimerHandle DestroyTimerHandle;
};
