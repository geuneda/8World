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
	
}

