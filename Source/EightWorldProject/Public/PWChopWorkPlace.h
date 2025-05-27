// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PWWorkPlaceInterface.h"
#include "GameFramework/Actor.h"
#include "PWChopWorkPlace.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APWChopWorkPlace : public AActor, public IPWWorkPlaceInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APWChopWorkPlace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//----------Interface--------------
	UPROPERTY(EditAnywhere)
	bool bIsReserved;

	UFUNCTION(BlueprintCallable)
	virtual bool IsAvailable() const override {return !bIsReserved;}
	
	virtual void Reserve(class APalYeti* PalWorker) override;
	virtual void Release() override;
	virtual void StartWork() override;

	//---------외형-------------
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UBoxComponent* BoxComp;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;
	
};
