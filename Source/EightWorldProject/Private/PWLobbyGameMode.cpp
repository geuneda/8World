// Fill out your copyright notice in the Description page of Project Settings.


#include "PWLobbyGameMode.h"

#include "PWGameInstance.h"

void APWLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UPWGameInstance>(GetGameInstance());
	
}
