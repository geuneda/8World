// Fill out your copyright notice in the Description page of Project Settings.


#include "PalAlpaca.h"

#include "PalWorkComponent.h"

// Sets default values
APalAlpaca::APalAlpaca()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Alpaca Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Alpaca/Alpaca.Alpaca'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-11.f), FRotator(0,-90,0));
	}

	PalworkComp = CreateDefaultSubobject<UPalWorkComponent>(TEXT("PalWorkComp"));
	
}

// Called when the game starts or when spawned
void APalAlpaca::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APalAlpaca::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APalAlpaca::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

