// Fill out your copyright notice in the Description page of Project Settings.


#include "Rock.h"

#include "Components/BoxComponent.h"


// Sets default values
ARock::ARock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetBoxExtent(FVector(130,170,160));
	RootComponent = Box;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Rock/Rock.Rock'"));
	if (tempMesh.Succeeded())
	{
		Mesh->SetStaticMesh(tempMesh.Object);
	}

	Tags.Add(FName("Rock"));
}

// Called when the game starts or when spawned
void ARock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

