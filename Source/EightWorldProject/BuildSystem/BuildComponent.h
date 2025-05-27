// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	int32 BuildD = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	FTransform BuildTransform;

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


public: // ---------------------머티리얼----------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterial* GreenMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterial* RedMaterial;



public: // ---------------------메시 종류 --------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildMesh")
	class UStaticMesh* Foundation;
	
};
