// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComp.h"

#include "PlayerCharacter.h"

// Sets default values for this component's properties
UPlayerStatComp::UPlayerStatComp()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UPlayerStatComp::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());

	HP = MaxHP;
	MP = MaxMP;
}


// Called every frame
void UPlayerStatComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 마나 리젠
	RegenMPTimer += DeltaTime;
	
	if (RegenMPTimer >= RegenMPInterval)
	{
		RegenMP();
		RegenMPTimer = 0;
	}
}

void UPlayerStatComp::RegenMP()
{
	// 휴식중이 아니면 회복하지 않음.
	if (!bIsRest) return;
	
	if (MP < MaxMP)
	{
		MP += 10;
		FMath::Clamp(MP, 0, MaxMP);
	}
}

