// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBox.h"

#include "Pal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/Resources/Rock.h"
#include "EightWorldProject/Resources/Tree.h"

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

	//시작때 감지된 모든 리소스 액터 담아두기(나무, 돌)
	GetWorldTimerManager().SetTimer(SearchResourceTimerHandle, this, &APalBox::SearchAllResources, 0.1f, false);
	//시작때 감지된 모든 팰 Worker 담아두기
	GetWorldTimerManager().SetTimer(SearchPalTimerHandle, this, &APalBox::SearchAllPalWorkers, 0.1f, false);
	//주기적으로 쉬고 있는 자원 체크
	GetWorldTimerManager().SetTimer(CheckRestResourceTimerHandle, this, &APalBox::CheckRestResources, 1.f, true);
	//주기적으로 쉬고 있는 팰 체크
	GetWorldTimerManager().SetTimer(CheckRestPalTimerHandle, this, &APalBox::CheckRestPals, 1.f, true);
}

// Called every frame
void APalBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void APalBox::SearchAllResources()
{
	TArray<AActor*> OverlappingResourceActors;
	SphereComp->GetOverlappingActors(OverlappingResourceActors, AActor::StaticClass());
	for (AActor* Actor : OverlappingResourceActors)
	{
		APal* pal = Cast<APal>(Actor);
		APlayerCharacter* player = Cast<APlayerCharacter>(Actor);
		ATree* tree = Cast<ATree>(Actor);
		ARock* rock = Cast<ARock>(Actor);
		if (Actor && Actor != this && !DetectedResourceActors.Contains(Actor) && !pal && !player)
		{
			if (tree)
			{
				DetectedResourceActors.Add(tree);
				UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllResources]Added Tree Actor : %s, Tree Actor IsBeingWorkedOn : %d"), *Actor->GetName(),tree->IsBeingWorkedOn());
			}
			if (rock)
			{
				DetectedResourceActors.Add(rock);
				UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllResources]Added Rock Actor : %s, Rock Actor IsBeingWorkedOn : %d"), *Actor->GetName(),rock->IsBeingWorkedOn());
			}
		}
	}
}

void APalBox::SearchAllPalWorkers()
{
	TArray<AActor*> OverlappingPalActors;
	SphereComp->GetOverlappingActors(OverlappingPalActors, APal::StaticClass());
	for (AActor* Actor : OverlappingPalActors)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(Actor);
		ATree* tree = Cast<ATree>(Actor);
		ARock* rock = Cast<ARock>(Actor);
		if (Actor && Actor != this && !DetectedPalWorkerActors.Contains(Actor) && !player && !tree && !rock)
		{
			APal* pal = Cast<APal>(Actor);
			if (pal && pal->bIsWorkerMode)
			{
				DetectedPalWorkerActors.Add(pal);
				UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllPalWorkers]Added Pal Actor : %s, Pal Actor IsWorking : %d"), *pal->GetName(), pal->GetPalIsWorking());
			}
		}
	}
}

void APalBox::CheckRestResources()
{
	for (AActor* Actor : DetectedResourceActors)
	{
		ATree* tree = Cast<ATree>(Actor);
		ARock* rock = Cast<ARock>(Actor);
		if (Actor && Actor != this)
		{
			if (tree)
			{
				if (!tree->IsBeingWorkedOn())
				{
					if (!RestResourceActors.Contains(tree))
					{
						RestResourceActors.Add(tree);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestResources] Tree Actor : %s, Tree Actor IsBeingWorkedOn : %d, New Added"), *Actor->GetName(),tree->IsBeingWorkedOn());
					}
				}
				else
				{
					if (RestResourceActors.Contains(tree))
					{
						RestResourceActors.Remove(tree);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestResources] Tree Actor : %s, Tree Actor IsBeingWorkedOn : %d, Removed"), *Actor->GetName(),tree->IsBeingWorkedOn());
					}
				}
			}
			if (rock)
			{
				if (!rock->IsBeingWorkedOn())
				{
					if (!RestResourceActors.Contains(rock))
					{
						RestResourceActors.Add(rock);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestResources] Rock Actor : %s, Rock Actor IsBeingWorkedOn : %d, New Added"), *Actor->GetName(),rock->IsBeingWorkedOn());
					}
				}
				else
				{
					if (RestResourceActors.Contains(rock))
					{
						RestResourceActors.Remove(rock);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestResources] Rock Actor : %s, Rock Actor IsBeingWorkedOn : %d, Removed"), *Actor->GetName(),rock->IsBeingWorkedOn());
					}
				}
			}
		}
	}
}

void APalBox::CheckRestPals()
{
	for (AActor* Actor : DetectedPalWorkerActors)
	{
		APal* pal = Cast<APal>(Actor);
		if (Actor && Actor != this)
		{
			if (pal)
			{
				if (!pal->GetPalIsWorking())
				{
					if (!RestPalActors.Contains(pal))
					{
						RestPalActors.Add(pal);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestPals] Pal Actor : %s, Pal Actor GetPalIsWorking : %d, New Added"), *Actor->GetName(),pal->GetPalIsWorking());
					}
				}
				else
				{
					if (RestPalActors.Contains(pal))
					{
						RestPalActors.Remove(pal);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestPals] Pal Actor : %s, Pal Actor GetPalIsWorking : %d, Removed"), *Actor->GetName(),pal->GetPalIsWorking());
					}
				}
			}
		}
	}
}

void APalBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APal* pal = Cast<APal>(OtherActor);
	APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
	ATree* tree = Cast<ATree>(OtherActor);
	ARock* rock = Cast<ARock>(OtherActor);
	//Resources 자원 (나무, 돌) 추가하기
	if (OtherActor && OtherActor != this && !pal && !player)
	{
		if (!DetectedResourceActors.Contains(OtherActor))
		{
			if (tree)
			{
				DetectedResourceActors.Add(tree);
				UE_LOG(PalBoxLog, Warning, TEXT("[OnBeginOverlap]Added Tree Actor : %s, Tree Actor IsBeingWorkedOn : %d"), *OtherActor->GetName(), tree->IsBeingWorkedOn());
			}
			if (rock)
			{
				DetectedResourceActors.Add(rock);
				UE_LOG(PalBoxLog, Warning, TEXT("[OnBeginOverlap]Added Rock Actor : %s, Rock Actor IsBeingWorkedOn : %d"), *OtherActor->GetName(), rock->IsBeingWorkedOn());
			}
		}
	}
	//팰 추가하기
	if (OtherActor && OtherActor != this && !player && !tree && !rock)
	{
		if (!DetectedPalWorkerActors.Contains(OtherActor))
		{
			if (pal)
			{
				DetectedPalWorkerActors.Add(pal);
				UE_LOG(PalBoxLog, Warning, TEXT("[OnBeginOverlap]Added Pal Actor : %s, Pal Actor IsWorking : %d"), *OtherActor->GetName(), pal->GetPalIsWorking());
			}
		}
	}
}

void APalBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//나무, 돌 자원이 사라지면 배열에서 제거
	if (OtherActor && DetectedResourceActors.Contains(OtherActor))
	{
		DetectedResourceActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Resource Actor : %s"), *OtherActor->GetName());
	}
	//쉬고 있는 자원 배열에서도 제거
	if (OtherActor && RestResourceActors.Contains(OtherActor))
	{
		RestResourceActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Rest Resource Actor : %s"), *OtherActor->GetName());
	}
	
	//팰이 사라지면 배열에서 제거
	if (OtherActor && DetectedPalWorkerActors.Contains(OtherActor))
	{
		DetectedPalWorkerActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Pal Actor : %s"), *OtherActor->GetName());
	}
	//쉬고 있는 팰 배열에서도 제거
	if (OtherActor && RestPalActors.Contains(OtherActor))
	{
		RestPalActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Rest Pal Actor : %s"), *OtherActor->GetName());
	}
}
