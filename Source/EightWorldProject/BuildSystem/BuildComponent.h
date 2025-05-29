// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Buildables.h"
#include "BuildComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EIGHTWORLDPROJECT_API UBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	// 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
	class UDataTable* BuildDataTable;

	// 빌드 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	TArray<FBuildData> BuildData;
	

	// 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	class UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	class UStaticMeshComponent* BuildGhostMeshComp;
	// 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	bool bIsBuildMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	bool bCanBuild = false;
	// 빌드 대상 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	int32 BuildID = 0;
	// 빌드 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	FTransform BuildTransform;
	// Owner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	class APlayerCharacter* PlayerCharacter;



public:
	
	void SpawnBuildGhost();

	void GiveBuildColor(bool bIsGreen);

	void BuildCycle();
	
	void ClearBuildGhostMesh();

	void BuildDelay();

	void LaunchBuildMode();

	void StopBuildMode();

	void ChangeMesh();

	void SpawnBuild();

	void DetectBuildBoxes();


public: // ---------------------머티리얼----------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterial* GreenMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterial* RedMaterial;



public: // ---------------------메시 종류 --------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildMesh")
	class UStaticMesh* Foundation;


public: // ---------------------스냅 시스템---------------------
	class AActor* HitActor;
	class UBoxComponent* HitComp;	
	
};
