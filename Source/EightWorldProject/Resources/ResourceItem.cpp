#include "ResourceItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"

AResourceItem::AResourceItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// 메시 컴포넌트 생성
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	// 물리 설정
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	// 충돌 컴포넌트 생성
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(Mesh);
	SphereCollider->SetSphereRadius(PickupRadius);
	SphereCollider->SetCollisionProfileName(TEXT("Item"));
	
	// 오버랩 이벤트 바인딩
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AResourceItem::OnOverlapBegin);
}

void AResourceItem::BeginPlay()
{
	Super::BeginPlay();
	
	DespawnTimer = 0.0f;
}

void AResourceItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 소멸 타이머 업데이트
	DespawnTimer += DeltaTime;
	
	// 자동 소멸 시간이 지나면 액터 파괴
	if (DespawnTimer >= DespawnTime)
	{
		Destroy();
	}
}

// 아이템 초기화
void AResourceItem::Initialize()
{
	// 아이템 크기 조정
	SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
	
	// 랜덤한 초기 속도 부여
	FVector RandomImpulse = FVector(
		FMath::RandRange(-100.0f, 100.0f),
		FMath::RandRange(-100.0f, 100.0f),
		FMath::RandRange(50.0f, 150.0f)
	);
	
	if (Mesh)
	{
		Mesh->AddImpulse(RandomImpulse);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[ResourceItem] 초기화 - ID: %s, 수량: %d"), *ResourceID.ToString(), Quantity);
}

// 아이템 습득
void AResourceItem::Pickup(AActor* Collector)
{
	// 플레이어 캐릭터인지 확인
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Collector);
	if (PlayerCharacter)
	{
		// 플레이어 캐릭터의 PickupItem 함수 호출
		PlayerCharacter->PickupItem(this);
		
		UE_LOG(LogTemp, Display, TEXT("아이템 획득 시도 : %s 수량 : %d"), *ResourceID.ToString(), Quantity);
	}
}

// 오버랩 이벤트 처리
void AResourceItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어와 오버랩 시 아이템 습득
	if (OtherActor && OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		Pickup(OtherActor);
	}
}
