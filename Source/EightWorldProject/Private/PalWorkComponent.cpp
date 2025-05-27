// Fill out your copyright notice in the Description page of Project Settings.


#include "PalWorkComponent.h"

#include "PalYeti.h"
#include "PWWorkPlaceInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPalWorkComponent::UPalWorkComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPalWorkComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPalWorkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UPalWorkComponent::FindAvailableWorkPlaceNearby()
{
	TArray<AActor*> TargetWorkPlaces;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UPWWorkPlaceInterface::StaticClass(), TargetWorkPlaces);

	for (AActor* Actor : TargetWorkPlaces)
	{
		IPWWorkPlaceInterface* WorkPlace = Cast<IPWWorkPlaceInterface>(Actor);
		if (WorkPlace->IsAvailable())
		{
			WorkPlace->Reserve(Cast<APalYeti>(GetOwner()));

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("WorkPlace : %s"), *(Actor->GetName())));
			return Actor;
		}
	}

	return nullptr;
}

