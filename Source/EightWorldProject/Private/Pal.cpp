// Fill out your copyright notice in the Description page of Project Settings.


#include "Pal.h"

#include "Components/BoxComponent.h"

// Sets default values
APal::APal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//박스 메쉬(임시)
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("'/Engine/EngineMeshes/SkeletalCube.SkeletalCube'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	
}

// Called when the game starts or when spawned
void APal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

