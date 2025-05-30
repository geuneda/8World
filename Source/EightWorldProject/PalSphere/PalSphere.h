// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

public:
	
	// 팰 스피어 피격 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class USphereComponent* SphereComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pal")
	class USkeletalMeshComponent* MeshComp;
};
