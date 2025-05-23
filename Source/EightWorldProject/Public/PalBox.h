// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PalBox.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APalBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APalBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//PalBox 외형
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	//PalBox 적용범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComp;

	//감지된 Actors 담기
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> DetectedActors;

	//Overlap 이벤트
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
				  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
