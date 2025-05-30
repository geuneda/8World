// Fill out your copyright notice in the Description page of Project Settings.

#include "PalSphereComponent.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "PlayerStatComp.h"
#include "EightWorldProject/PalSphere/PalSphere.h"

UPalSphereComponent::UPalSphereComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPalSphereComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 소유자 캐릭터 설정
	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
}

void UPalSphereComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// 팰스피어 던지기 함수
void UPalSphereComponent::ThrowPalSphere()
{
	if (!OwnerCharacter)
	{
		return;
	}

	// 팰스피어 소환
	APalSphere* PalSphere = SpawnPalSphere();
	if (!PalSphere)
	{
		return;
	}

	// 던질 방향 계산
	FVector ThrowDirection = CalculateThrowDirection();
	
	// 팰스피어 발사
	PalSphere->FireInDirection(ThrowDirection);
	
	// 휴식 상태로 변경 (애니메이션 완료 후)
	FTimerHandle RestStateTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RestStateTimerHandle, [this]()
	{
		if (OwnerCharacter && OwnerCharacter->PlayerStatComp)
		{
			OwnerCharacter->PlayerStatComp->SetRestState(true);
		}
	}, 1.0f, false);
}

// 팰스피어 소환 함수
APalSphere* UPalSphereComponent::SpawnPalSphere()
{
	if (!OwnerCharacter || !PalSphereClass)
	{
		return nullptr;
	}

	// 소켓 위치 가져오기
	FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(PalSocketName);
	FRotator SocketRotation = OwnerCharacter->GetMesh()->GetSocketRotation(PalSocketName);

	// 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 팰스피어 스폰
	APalSphere* SpawnedPalSphere = GetWorld()->SpawnActor<APalSphere>(
		PalSphereClass,
		SocketLocation,
		SocketRotation,
		SpawnParams
	);

	return SpawnedPalSphere;
}

// 크로스헤어 위치 계산 함수
FVector UPalSphereComponent::GetCrosshairWorldLocation()
{
	if (!OwnerCharacter)
	{
		return FVector::ZeroVector;
	}

	// 플레이어 컨트롤러와 카메라 가져오기
	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}

	// 뷰포트 크기 가져오기
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// 화면 중앙 위치 (크로스헤어 위치)
	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	// 화면 위치를 월드 위치로 변환
	FVector WorldLocation, WorldDirection;
	if (PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		// 라인 트레이스로 크로스헤어가 가리키는 지점 찾기
		FHitResult HitResult;
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = WorldLocation + WorldDirection * ThrowDistance;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerCharacter);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			return HitResult.Location;
		}
		else
		{
			// 아무것도 맞지 않았을 경우 최대 거리 지점 반환
			return TraceEnd;
		}
	}

	return FVector::ZeroVector;
}

// 던질 방향 계산 함수
FVector UPalSphereComponent::CalculateThrowDirection()
{
	if (!OwnerCharacter)
	{
		return FVector::ZeroVector;
	}

	// 소켓 위치 가져오기
	FVector SocketLocation = OwnerCharacter->GetMesh()->GetSocketLocation(PalSocketName);
	
	// 크로스헤어 위치 가져오기
	FVector TargetLocation = GetCrosshairWorldLocation();
	
	// 높이 오프셋 적용 (포물선 궤적을 위해)
	TargetLocation.Z += ThrowHeightOffset;
	
	// 던질 방향 계산 (정규화된 벡터)
	FVector ThrowDirection = (TargetLocation - SocketLocation).GetSafeNormal();
	
	return ThrowDirection;
}
