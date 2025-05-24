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

	//TimerHandle 자원, 팰
	FTimerHandle SearchResourceTimerHandle;
	FTimerHandle SearchPalTimerHandle;
	
	//TimerHandle 쉬고있는 자원들, 팰 체크
	FTimerHandle CheckRestResourceTimerHandle;
	FTimerHandle CheckRestPalTimerHandle;
	
	//감지된 Actors 담기 자원, 팰
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> DetectedResourceActors;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> DetectedPalWorkerActors;

	//팰이 작업중이지 않은 자원, 팰 담는 배열
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> RestResourceActors;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<AActor*> RestPalActors;

	//초기 Resource 찾아 저장하는 함수
	UFUNCTION()
	void SearchAllResources();
	//초기 PalWorker 찾아 저장하는 함수
	UFUNCTION()
	void SearchAllPalWorkers();

	//주기적으로 쉬고 있는 자원, 팰 체크 함수
	UFUNCTION()
	void CheckRestResources();
	UFUNCTION()
	void CheckRestPals();
	
	//Overlap 이벤트
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
						const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
				  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
