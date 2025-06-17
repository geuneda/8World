// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComp.h"

#include "PlayerCharacter.h"
#include "EightWorldProject/EightWorldProject.h"
#include "EightWorldProject/UI/MainUI.h"
#include "EightWorldProject/UI/MPUI.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerStatComp::UPlayerStatComp()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPlayerStatComp::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());

	// Initialize HP and MP to their maximum values
	if (Player && Player->HasAuthority())
	{
		SetHP(MaxHP);
		SetMP(MaxMP);
	}
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
	//PRINTLOG(TEXT("=============> MP : %f"), MP);
	OnRep_MP();
	// if (Player && Player->MainUI && Player->MainUI->MPUI)
	// {
	// 	Player->MainUI->MPUI->SetMP(GetMP(), GetMaxMP());
	// }
}

void UPlayerStatComp::OnRep_MP()
{
	//PRINTLOG( TEXT("MP : %f, parent : %p, child : %p"), GetMP(), Player, this);
	if (Player && Player->MainUI && Player->MainUI->MPUI)
	{
		Player->MainUI->MPUI->SetMP(MP, GetMaxMP());
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

void UPlayerStatComp::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerStatComp, bIsRest);
	DOREPLIFETIME(UPlayerStatComp, MP);
	
}

