// Fill out your copyright notice in the Description page of Project Settings.


#include "PalSphere.h"

#include "Components/SphereComponent.h"


// Sets default values
APalSphere::APalSphere()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(10);
	SetRootComponent(SphereComp);
	
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/PalSphere/PalSphere.PalSphere'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetSkeletalMesh(tempMesh.Object);
	}

	MeshComp->SetRelativeScale3D(FVector(0.02f));
}

// Called when the game starts or when spawned
void APalSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APalSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

