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
	
public: //--------------- Widget---------------
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UGoalWidget> goalUIWidget;
	UPROPERTY()
	class UGoalWidget* goalWidget;
};
