// Fill out your copyright notice in the Description page of Project Settings.

#include "PalSphere.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "EightWorldProject/Public/Pal.h"

// Sets default values
APalSphere::APalSphere()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 구체 컴포넌트 생성 및 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(10.0f);
	SphereComp->SetCollisionProfileName(TEXT("PalSphere"));
	SphereComp->SetSimulatePhysics(false);
	SphereComp->SetNotifyRigidBodyCollision(true); // 물리 충돌 이벤트 활성화
	SetRootComponent(SphereComp);
	
	// 메시 컴포넌트 생성 및 설정
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 메시 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/PalSphere/PalSphere.PalSphere'"));
	if (tempMesh.Succeeded())
	{
		MeshComp->SetSkeletalMesh(tempMesh.Object);
	}

	MeshComp->SetRelativeScale3D(FVector(0.02f));

	// 발사체 컴포넌트 생성 및 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;

	// 충돌 이벤트 바인딩
	SphereComp->OnComponentHit.AddDynamic(this, &APalSphere::OnSphereHit);
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

// 충돌 처리 함수
void APalSphere::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 충돌 후 물리 시뮬레이션 활성화
	EnablePhysicsSimulation();

	// 충돌한 액터가 Pal인지 확인
	APal* HitPal = Cast<APal>(OtherActor);
	if (HitPal)
	{
		// 팰 포획 처리
		HitPal->CaptureByPlayer();

		// 포획 이펙트 재생
		if (CaptureEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CaptureEffect, GetActorLocation(), GetActorRotation());
		}

		// 팰스피어 파괴
		Destroy();
	}
	// Pal이 아닌 경우 일정 시간 후 파괴
	else
	{
		SetDestroyTimer();
	}
}

// 팰스피어 발사 함수
void APalSphere::FireInDirection(const FVector& Direction)
{
	ProjectileMovement->Velocity = Direction * InitialSpeed;
}

// 팰스피어 물리 시뮬레이션 활성화
void APalSphere::EnablePhysicsSimulation()
{
	// 발사체 컴포넌트 비활성화
	ProjectileMovement->SetActive(false);

	// 물리 시뮬레이션 활성화
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetEnableGravity(true);
}

// 팰스피어 자동 파괴 타이머 설정
void APalSphere::SetDestroyTimer()
{
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &APalSphere::K2_DestroyActor, DestroyTime, false);
}

