// Fill out your copyright notice in the Description page of Project Settings.


#include "FoundationStaticMeshActor.h"

#include "Components/BoxComponent.h"


// Sets default values
AFoundationStaticMeshActor::AFoundationStaticMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box1->SetRelativeLocation(FVector(-400.f, 0.f, 0.f));
	Box2->SetRelativeLocation(FVector(400.f, 0.f, 0.f));
	Box3->SetRelativeLocation(FVector(0.f, -400.f, 0.f));
	Box4->SetRelativeLocation(FVector(0.f, 400.f, 0.f));

	Box1->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	Box2->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	Box3->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	Box4->SetBoxExtent(FVector(200.f, 200.f, 100.f));

	Box1->SetCollisionProfileName("FoundationTrace");
	Box2->SetCollisionProfileName("FoundationTrace");
	Box3->SetCollisionProfileName("FoundationTrace");
	Box4->SetCollisionProfileName("FoundationTrace");
}

// Called when the game starts or when spawned
void AFoundationStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoundationStaticMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<class UBoxComponent*> AFoundationStaticMeshActor::ReturnBoxes()
{
	TArray<UBoxComponent*> Boxes;
	Boxes.Add(Box1);
	Boxes.Add(Box2);
	Boxes.Add(Box3);
	Boxes.Add(Box4);

	return Boxes;
}

