// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EightWorldProject/Interface/BuildInterface.h"
#include "Engine/StaticMeshActor.h"
#include "BaseStaticMeshActor.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API ABaseStaticMeshActor : public AStaticMeshActor, public IBuildInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseStaticMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual TArray<class UBoxComponent*> ReturnBoxes() override;

	class USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Box4;
};
