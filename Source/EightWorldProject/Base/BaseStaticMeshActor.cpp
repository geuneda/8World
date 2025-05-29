// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStaticMeshActor.h"

#include "Components/BoxComponent.h"


// Sets default values
ABaseStaticMeshActor::ABaseStaticMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SceneComp->SetupAttachment(RootComponent);
	
	Box1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Box1"));
	Box2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Box2"));
	Box3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Box3"));
	Box4 = CreateDefaultSubobject<UBoxComponent>(TEXT("Box4"));

	Box1->SetupAttachment(SceneComp);
	Box2->SetupAttachment(SceneComp);
	Box3->SetupAttachment(SceneComp);
	Box4->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void ABaseStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseStaticMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<class UBoxComponent*> ABaseStaticMeshActor::ReturnBoxes()
{
	TArray<UBoxComponent*> Boxes;
	Boxes.Add(Box1);
	Boxes.Add(Box2);
	Boxes.Add(Box3);
	Boxes.Add(Box4);

	return Boxes;
}