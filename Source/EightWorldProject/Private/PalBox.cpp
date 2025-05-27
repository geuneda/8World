// Fill out your copyright notice in the Description page of Project Settings.


#include "PalBox.h"

#include "Pal.h"
#include "PWGameMode.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/Resources/ResourceItem.h"
#include "EightWorldProject/Resources/ResourceManager.h"
#include "EightWorldProject/Resources/Rock.h"
#include "EightWorldProject/Resources/Tree.h"
#include "Kismet/GameplayStatics.h"

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
	SphereComp->SetSphereRadius(6000.f);
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
	GetWorldTimerManager().SetTimer(CheckRestResourceTimerHandle, this, &APalBox::CheckRestResources, 0.5f, true);
	//주기적으로 쉬고 있는 팰 체크
	GetWorldTimerManager().SetTimer(CheckRestPalTimerHandle, this, &APalBox::CheckRestPals, 0.5f, true);

	//시작때 감지된 모든 팰 Carrier 담아두기
	GetWorldTimerManager().SetTimer(SearchCarrierTimerHandle, this, &APalBox::SearchAllPalCarriers, 0.1f, false);
	//주기적으로 떨어진 아이템 체크
	GetWorldTimerManager().SetTimer(CheckDroppedItemTimerHandle, this, &APalBox::CheckDroppedItems, 0.5f, true);
	//주기적으로 쉬고 있는 아이템 체크
	GetWorldTimerManager().SetTimer(CheckRestItemTimerHandle, this, &APalBox::CheckRestItems, 0.5f, true);
	//주기적으로 쉬고 있는 팰 체크
	GetWorldTimerManager().SetTimer(CheckRestCarrierTimerHandle, this, &APalBox::CheckRestCarriers, 0.5f, true);

	
}

// Called every frame
void APalBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void APalBox::SearchAllResources()
{
	// 기존 배열 초기화
	DetectedResourceActors.Empty();

	// 구체적인 타입으로 바로 검색 (Tree)
	TArray<AActor*> OverlappingTrees;
	SphereComp->GetOverlappingActors(OverlappingTrees, ATree::StaticClass());
	for (AActor* Actor : OverlappingTrees)
	{
		ATree* Tree = Cast<ATree>(Actor);
		if (Tree)
		{
			DetectedResourceActors.Add(Tree);
			UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllResources] Added Tree Actor: %s, IsBeingWorkedOn: %d"), 
				*Tree->GetName(), Tree->IsBeingWorkedOn());
		}
	}

	// 구체적인 타입으로 바로 검색 (Rock)
	TArray<AActor*> OverlappingRocks;
	SphereComp->GetOverlappingActors(OverlappingRocks, ARock::StaticClass());
	for (AActor* Actor : OverlappingRocks)
	{
		ARock* Rock = Cast<ARock>(Actor);
		if (Rock)
		{
			DetectedResourceActors.Add(Rock);
			UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllResources] Added Rock Actor: %s, IsBeingWorkedOn: %d"), 
				*Rock->GetName(), Rock->IsBeingWorkedOn());
		}
	}
}

void APalBox::SearchAllPalWorkers()
{
	TArray<AActor*> OverlappingPalActors;
	SphereComp->GetOverlappingActors(OverlappingPalActors, APal::StaticClass());
	for (AActor* Actor : OverlappingPalActors)
	{
		if (Actor && Actor != this && !DetectedPalWorkerActors.Contains(Actor))
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
	
	FindNearResourceAndPal();
}

void APalBox::FindNearResourceAndPal()
{
	FVector PalBoxLocation = this->GetActorLocation();
	FVector TargetResourceLocation = FVector::ZeroVector;
	float dist =  100000.f;
	AActor* NearResourceActor = nullptr;
	APal* pal = nullptr;
	
	//쉬는 팰 존재시 랜덤으로 하나 잡기
	if (RestPalActors.Num() > 0)
	{
		int32 index = FMath::RandRange(0,RestPalActors.Num()-1);
		pal = Cast<APal>(RestPalActors[index]);
		//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearResourceAndPal] Pal : %s"), *pal->GetName());
	}
	//가장 가까운 자원 찾기
	for (AActor* Actor : RestResourceActors)
	{
		if (Actor && Actor != this)
		{
			TargetResourceLocation = Actor->GetActorLocation();
			if (dist >= FVector::Dist(PalBoxLocation, TargetResourceLocation))
			{
				dist = FVector::Dist(PalBoxLocation, TargetResourceLocation);
				NearResourceActor = Actor;
			}
		}
	}
	if (NearResourceActor)
	{
		//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearResourceAndPal] Actor : %s"), *NearResourceActor->GetName());
	}

	if (pal && NearResourceActor)
	{
		//쉬고 있는 자원과 팰 배열에서 제거
		if (RestPalActors.Contains(pal))
		{
			RestPalActors.Remove(pal);
			pal->SetPalIsWorking(true); //작업중인 팰로 변경
			//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearResourceAndPal] Pal Actor : %s, Pal Actor GetPalIsWorking : %d, RestPalActors Removed"), *pal->GetName(),pal->GetPalIsWorking());
		}
		if (RestResourceActors.Contains(NearResourceActor))
		{
			RestResourceActors.Remove(NearResourceActor);
			//작업당하는 자원으로 변경
			if (ATree* tree = Cast<ATree>(NearResourceActor))
			{
				tree->SetIsBeingWorkedOn(true);
				//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearResourceAndPal] Tree Actor : %s, Tree Actor IsBeingWorkedOn : %d, RestResourceActors Removed"), *tree->GetName(),tree->IsBeingWorkedOn());
			}
			else if (ARock* rock = Cast<ARock>(NearResourceActor))
			{
				rock->SetIsBeingWorkedOn(true);
				//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearResourceAndPal] Rock Actor : %s, Rock Actor IsBeingWorkedOn : %d, RestResourceActors Removed"), *rock->GetName(),rock->IsBeingWorkedOn());
			}
		}
		
		MakePalWorkToResource(NearResourceActor, pal);
	}
}

void APalBox::MakePalWorkToResource(AActor* resource, APal* palWorker)
{
	if (resource && palWorker)
	{
		//작업중인 팰, 자원 배열에 추가
		if (!WorkedResourceActors.Contains(resource))
		{
			WorkedResourceActors.Add(resource);
			UE_LOG(PalBoxLog, Warning, TEXT("[MakePalWorkToResource] Added Worked Resource : %s"), *resource->GetName());
		}
		if (!WorkedPalActors.Contains(palWorker))
		{
			WorkedPalActors.Add(palWorker);
			UE_LOG(PalBoxLog, Warning, TEXT("[MakePalWorkToResource] Added Worked Pal : %s"), *palWorker->GetName());
		}
		//팰 작업 자원 찾기 시작 상태변경
		palWorker->SetPalWorkerState(EPalWorkerState::FindWork, resource);
		UE_LOG(PalBoxLog, Warning, TEXT("[MakePalWorkToResource] Start Working / Resource : %s, PalWorker : %s"), *resource->GetName(), *palWorker->GetName());
	}
}

void APalBox::CheckDroppedItems()
{
	//UE_LOG(PalBoxLog, Warning, TEXT("[CheckDroppedItems] Check"));
	//PalBox Sphere 범위안에 Item들 찾기
	TArray<AActor*> OverlappingDroppedItems;
	SphereComp->GetOverlappingActors(OverlappingDroppedItems, AResourceItem::StaticClass());
	//하나라도 있으면
	if (OverlappingDroppedItems.Num() > 0)
	{
		for (AActor* Item : OverlappingDroppedItems)
		{
			//저장되지 않은 아이템들 찾기
			if (Item && !DroppedItemActors.Contains(Item))
			{	
				DroppedItemActors.Add(Item);
				UE_LOG(PalBoxLog, Warning, TEXT("[CheckDroppedItems]Added Item Actor : %s"), *Item->GetName());
			}
		}
	}
}

void APalBox::SearchAllPalCarriers()
{
	TArray<AActor*> OverlappingPalActors;
	SphereComp->GetOverlappingActors(OverlappingPalActors, APal::StaticClass());
	for (AActor* Actor : OverlappingPalActors)
	{
		if (Actor && !DetectedPalCarrierActors.Contains(Actor))
		{
			APal* pal = Cast<APal>(Actor);
			if (pal && pal->bIsCarrierMode)
			{
				DetectedPalCarrierActors.Add(pal);
				UE_LOG(PalBoxLog, Warning, TEXT("[SearchAllPalCarriers]Added Pal Actor : %s, Pal Actor IsCarrying : %d"), *pal->GetName(), pal->GetPalIsCarrying());
			}
		}
	}
}

void APalBox::CheckRestItems()
{
	for (AActor* Actor : DroppedItemActors)
	{
		if (Actor)
		{
			AResourceItem* item = Cast<AResourceItem>(Actor);
			if (item)
			{
				if (!item->IsBeingCarriedOn())
				{
					if (!RestItemActors.Contains(item))
					{
						RestItemActors.Add(item);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestItems] Item Actor : %s, Item Actor IsBeingCarriedOn : %d, New Added"), *item->GetName(),item->IsBeingCarriedOn());
					}
				}
				else
				{
					if (RestItemActors.Contains(item))
					{
						RestItemActors.Remove(item);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestItems] Item Actor : %s, Item Actor IsBeingCarriedOn : %d, Removed"), *item->GetName(),item->IsBeingCarriedOn());
					}
				}
			}
			
		}
	}
}

void APalBox::CheckRestCarriers()
{
	for (AActor* Actor : DetectedPalCarrierActors)
	{
		if (Actor)
		{
			APal* pal = Cast<APal>(Actor);
			if (pal)
			{
				if (!pal->GetPalIsCarrying())
				{
					if (!RestCarrierActors.Contains(pal))
					{
						RestCarrierActors.Add(pal);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestCarriers] Pal Actor : %s, Pal Actor GetPalIsCarrying : %d, New Added"), *pal->GetName(),pal->GetPalIsCarrying());
					}
				}
				else
				{
					if (RestCarrierActors.Contains(pal))
					{
						RestCarrierActors.Remove(pal);
						UE_LOG(PalBoxLog, Warning, TEXT("[CheckRestCarriers] Pal Actor : %s, Pal Actor GetPalIsCarrying : %d, Removed"), *pal->GetName(),pal->GetPalIsCarrying());
					}
				}
			}
		}
	}
	
	FindNearDroppedItem();
}

void APalBox::FindNearDroppedItem()
{
	FVector PalBoxLocation = this->GetActorLocation();
	FVector TargetItemLocation = FVector::ZeroVector;
	float dist =  100000.f;
	AActor* NearItemActor = nullptr;
	APal* pal = nullptr;

	//쉬는 운반용 팰 존재시 랜덤으로 하나 잡기
	if (RestCarrierActors.Num() > 0)
	{
		int32 index = FMath::RandRange(0,RestCarrierActors.Num()-1);
		pal = Cast<APal>(RestCarrierActors[index]);
		//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearDroppedItem] Pal : %s"), *pal->GetName());
	}
	//가장 가까운 자원 찾기
	for (AActor* Actor : RestItemActors)
	{
		if (Actor)
		{
			TargetItemLocation = Actor->GetActorLocation();
			if (dist >= FVector::Dist(PalBoxLocation, TargetItemLocation))
			{
				dist = FVector::Dist(PalBoxLocation, TargetItemLocation);
				NearItemActor = Actor;
			}
		}
	}
	if (NearItemActor)
	{
		//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearDroppedItem] Actor : %s"), *NearItemActor->GetName());
	}

	if (pal && NearItemActor)
	{
		//쉬고 있는 자원과 팰 배열에서 제거
		if (RestCarrierActors.Contains(pal))
		{
			RestCarrierActors.Remove(pal);
			pal->SetPalIsCarrying(true); //운반중인 팰로 변경
			//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearDroppedItem] Pal Actor : %s, Pal Actor GetPalIsCarrying : %d, RestCarrierActors Removed"), *pal->GetName(),pal->GetPalIsCarrying());
		}
		if (RestItemActors.Contains(NearItemActor))
		{
			RestItemActors.Remove(NearItemActor);
			//작업당하는 자원으로 변경
			if (AResourceItem* item = Cast<AResourceItem>(NearItemActor))
			{
				item->SetIsBeinngCarriedOn(true);
				//UE_LOG(PalBoxLog, Warning, TEXT("[FindNearDroppedItem] Item Actor : %s, Item Actor IsBeingCarriedOn : %d, RestItemActors Removed"), *item->GetName(),item->IsBeingCarriedOn());
			}
		}
		
		MakePalCarryToItem(NearItemActor, pal);
	}
}

void APalBox::MakePalCarryToItem(AActor* item, APal* palCarrier)
{
	if (item && palCarrier)
	{
		// //운반중인 팰, 아이템 배열에 추가
		// if (!WorkedResourceActors.Contains(resource))
		// {
		// 	WorkedResourceActors.Add(resource);
		// 	UE_LOG(PalBoxLog, Warning, TEXT("[MakePalWorkToResource] Added Worked Resource : %s"), *resource->GetName());
		// }
		// if (!WorkedPalActors.Contains(palWorker))
		// {
		// 	WorkedPalActors.Add(palWorker);
		// 	UE_LOG(PalBoxLog, Warning, TEXT("[MakePalWorkToResource] Added Worked Pal : %s"), *palWorker->GetName());
		// }
		
		//팰 운반 아이템 찾기 시작 상태변경
		palCarrier->SetPalCarrierState(EPalCarrierState::FindItem, item);
		//UE_LOG(PalBoxLog, Warning, TEXT("[MakePalCarryToItem] Start Carrying / Item : %s, PalCarrier : %s"), *item->GetName(), *palCarrier->GetName());
		
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
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Detected Resource Actor : %s"), *OtherActor->GetName());
	}
	//쉬고 있는 자원 배열에서도 제거
	if (OtherActor && RestResourceActors.Contains(OtherActor))
	{
		RestResourceActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Rest Resource Actor : %s"), *OtherActor->GetName());
	}
	//만약 작업중이었던 자원이면 배열에서도 제거
	{
		if (OtherActor && WorkedResourceActors.Contains(OtherActor))
		{
			WorkedResourceActors.Remove(OtherActor);
			UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Worked Resource Actor : %s"), *OtherActor->GetName());
		}
	}
	
	//팰이 사라지면 배열에서 제거
	if (OtherActor && DetectedPalWorkerActors.Contains(OtherActor))
	{
		DetectedPalWorkerActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Detected Pal Actor : %s"), *OtherActor->GetName());
	}
	//쉬고 있는 팰 배열에서도 제거
	if (OtherActor && RestPalActors.Contains(OtherActor))
	{
		RestPalActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Rest Pal Actor : %s"), *OtherActor->GetName());
	}
	//만약 작업중이었던 팰이면 배열에서도 제거
	if (OtherActor && WorkedPalActors.Contains(OtherActor))
	{
		WorkedPalActors.Remove(OtherActor);
		UE_LOG(PalBoxLog, Warning, TEXT("[OnEndOverlap]Removed Worked Pal Actor : %s"), *OtherActor->GetName());
	}
}
