// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PWPlayerController.generated.h"


UCLASS()
class EIGHTWORLDPROJECT_API APWPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APWPlayerController();

	
public: //--------------UI----------------
	virtual void BeginPlay() override;
	
	UPROPERTY()
	class APlayerCharacter* myPlayer;

	UPROPERTY()
	class UGoalWidget* goalWidget;
	UPROPERTY()
	class UMissionCompleteWidget* missionCompleteWidget;
};
