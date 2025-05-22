// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EIGHTWORLDPROJECT_API UPlayerStatComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	// 플레이어
	class APlayerCharacter* Player;
	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HP)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HP)
	float MaxHP;
	
	// 기력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP)
	float MP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP)
	float MaxMP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP)
	float RegenMPInterval = 1.0f;
	float RegenMPTimer;
	
	// 휴식중인지 여부
	bool bIsRest;

public:
	void RegenMP();
};
