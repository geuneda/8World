// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuildInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBuildInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EIGHTWORLDPROJECT_API IBuildInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual TArray<class UBoxComponent*> ReturnBoxes() = 0;
};
