// Fill out your copyright notice in the Description page of Project Settings.


#include "PWChopWorkPlace.h"

#include "Components/BoxComponent.h"


// Sets default values
APWChopWorkPlace::APWChopWorkPlace()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetBoxExtent(FVector(50.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void APWChopWorkPlace::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APWChopWorkPlace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APWChopWorkPlace::Reserve(class APalAlpaca* PalWorker)
{
}

void APWChopWorkPlace::Release()
{
}

void APWChopWorkPlace::StartWork()
{
}
