// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rock.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API ARock : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 스태틱 메시
	class UStaticMeshComponent* Mesh;
	// 충돌체
	class UBoxComponent* Box;
};
