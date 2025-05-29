// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EightWorldProject/Base/BaseStaticMeshActor.h"
#include "FloorStaticMeshActor.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API AFloorStaticMeshActor : public ABaseStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFloorStaticMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual TArray<class UBoxComponent*> ReturnBoxes() override;
};
