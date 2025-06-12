// Fill out your copyright notice in the Description page of Project Settings.


#include "PWPlayerController.h"
#include "GoalWidget.h"
#include "PWGameInstance.h"
#include "Components/CheckBox.h"
#include "EightWorldProject/Player/PlayerCharacter.h"

APWPlayerController::APWPlayerController()
{
	
}

void APWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	myPlayer = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

