// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonStorageBox.h"

#include "Components/BoxComponent.h"

// Sets default values
ACommonStorageBox::ACommonStorageBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetBoxExtent(FVector(45.f,45.f,40.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);
	MeshComp->SetRelativeLocationAndRotation(FVector(-40.f, 40.f, -40.f), FRotator(0.f));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/PalWorld/Box/wood-box/source/wood_box.wood_box'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(tempMesh.Object);
	}

}

// Called when the game starts or when spawned
void ACommonStorageBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACommonStorageBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

