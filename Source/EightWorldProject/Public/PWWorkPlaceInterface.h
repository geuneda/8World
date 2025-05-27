// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PWWorkPlaceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPWWorkPlaceInterface : public UInterface
{
	GENERATED_BODY()
};

class EIGHTWORLDPROJECT_API IPWWorkPlaceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool IsAvailable() const = 0;
	virtual void Reserve(class APalYeti* PalWorker) = 0;
	virtual void Release() = 0;
	virtual void StartWork() = 0;
};
