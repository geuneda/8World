// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComp.h"

#include "PlayerCharacter.h"
#include "EightWorldProject/UI/MainUI.h"
#include "EightWorldProject/UI/MPUI.h"

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

	// Initialize HP and MP to their maximum values
	SetHP(MaxHP);
	SetMP(MaxMP);
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

void UPlayerStatComp::SetHP(float NewHP)
{
	HP = FMath::Clamp(NewHP, 0.0f, MaxHP);
}

void UPlayerStatComp::SetMP(float NewMP)
{
	MP = FMath::Clamp(NewMP, 0.0f, MaxMP);

	if (Player && Player->MainUI && Player->MainUI->MPUI)
	{
		Player->MainUI->MPUI->SetMP(GetMP(), GetMaxMP());
	}
}

void UPlayerStatComp::RegenMP()
{
	// 휴식중이 아니면 회복하지 않음.
	if (!IsResting()) return;
	
	if (GetMP() < GetMaxMP())
	{
		SetMP(GetMP() + 10);
	}
}

