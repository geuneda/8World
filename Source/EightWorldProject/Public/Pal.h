// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Pal.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API APal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//팰 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PalName;
	//팰 일종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WorkType;
	//팰 일속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorkSpeed;
	//팰 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* WorkMontage;
};
