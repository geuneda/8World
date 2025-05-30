// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PalSphereComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EIGHTWORLDPROJECT_API UPalSphereComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPalSphereComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 팰스피어 던지기 함수
	void ThrowPalSphere();

	// 팰스피어 소환 함수
	class APalSphere* SpawnPalSphere();

	// 크로스헤어 위치 계산 함수
	FVector GetCrosshairWorldLocation();

	// 던질 방향 계산 함수
	FVector CalculateThrowDirection();

public:
	// 팰스피어 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalSphere")
	TSubclassOf<class APalSphere> PalSphereClass;

	// 팰스피어 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalSphere")
	FName PalSocketName = "PalSocket";

	// 팰스피어 던지기 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalSphere")
	float ThrowDistance = 2000.0f;

	// 팰스피어 던지기 높이 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PalSphere")
	float ThrowHeightOffset = 50.0f;

	// 소유자 캐릭터
	UPROPERTY()
	class APlayerCharacter* OwnerCharacter;
};
