// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBox.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

DECLARE_LOG_CATEGORY_EXTERN(PalBoxLog, Log, All);
DEFINE_LOG_CATEGORY(PalBoxLog);

// Sets default values
APalBox::APalBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetBoxExtent(FVector(30.f, 90.f, 95.f));
	BoxComp->SetCollisionProfileName("BlockAll");

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);
	MeshComp->SetRelativeLocation(FVector(-6.f, 0, -94.5f));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempPalBox(TEXT("'/Game/Resources/Build/PalBox/PalBox.PalBox'"));
	if (tempPalBox.Succeeded())
	{
		MeshComp->SetStaticMesh(tempPalBox.Object);
	}

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(BoxComp);
	SphereComp->SetSphereRadius(5000.f);
	SphereComp->SetCollisionProfileName("PalBox");
	
}

// Called when the game starts or when spawned
void APalBox::BeginPlay()
{
	Super::BeginPlay();

	//Overlap 바인딩
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &APalBox::OnBeginOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &APalBox::OnEndOverlap);
}

// Called every frame
void APalBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void APalBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (!DetectedActors.Contains(OtherActor))
		{
			DetectedActors.Add(OtherActor);
			UE_LOG(PalBoxLog, Warning, TEXT("Added Actor : %s"), *OtherActor->GetName());
		}
	}
}

void APalBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor && DetectedActors.Contains(OtherActor))
	{
		DetectedActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("Removed Actor : %s"), *OtherActor->GetName());
	}
}
