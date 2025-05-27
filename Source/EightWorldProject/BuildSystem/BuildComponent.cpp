#include "BuildComponent.h"

#include "Camera/CameraComponent.h"
#include "EightWorldProject/Player/PlayerCharacter.h"

UBuildComponent::UBuildComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UMaterial> GreenMaterialObj(TEXT("/Script/Engine.Material'/Game/BaseBuilding/M_GreenColor.M_GreenColor'"));
	if (GreenMaterialObj.Succeeded())
	{
		GreenMaterial = GreenMaterialObj.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> RedMaterialObj(TEXT("/Script/Engine.Material'/Game/BaseBuilding/M_RedColor.M_RedColor'"));
	if (RedMaterialObj.Succeeded())
	{
		RedMaterial = RedMaterialObj.Object;
	}


	ConstructorHelpers::FObjectFinder<UStaticMesh> FoundationObj(TEXT("/Script/Engine.StaticMesh'/Game/BaseBuilding/Build/foundation.foundation'"));
	if (FoundationObj.Succeeded())
	{
		Foundation = FoundationObj.Object;
	}
}


// Called when the game starts
void UBuildComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}


// Called every frame
void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBuildComponent::SpawnBuildGhost()
{
	BuildGhostMeshComp = Cast<UStaticMeshComponent>(PlayerCharacter->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, BuildTransform, false));

	BuildGhostMeshComp->SetStaticMesh(Foundation);

	BuildGhostMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UBuildComponent::GiveBuildColor(bool bIsGreen)
{
	bCanBuild = bIsGreen;
	

	for (int8 i = 0; i < BuildGhostMeshComp->GetNumMaterials(); i++)
	{
		if (bCanBuild)
		{
			BuildGhostMeshComp->SetMaterial(i, GreenMaterial);
		}
		else
		{
			BuildGhostMeshComp->SetMaterial(i, RedMaterial);
		}
	}

	BuildGhostMeshComp->SetWorldTransform(BuildTransform);
}

void UBuildComponent::BuildCycle()
{
	FVector cameraLot = CameraComp->GetComponentLocation();
	FVector cameraDir = CameraComp->GetForwardVector();

	FVector start = cameraLot + (cameraDir * 350);
	FVector end = cameraLot + (cameraDir * 1000);

	FHitResult hitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this->GetOwner());
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, Params);

	if (bHit)
	{
		BuildTransform.SetLocation(hitResult.ImpactPoint);
		
		if (BuildGhostMeshComp)
		{
			GiveBuildColor(true);

			BuildDelay();
		}
		else
		{
			SpawnBuildGhost();

			BuildDelay();
		}
	}
	else
	{
		BuildTransform.SetLocation(hitResult.TraceEnd);

		if (BuildGhostMeshComp)
		{
			GiveBuildColor(false);

			BuildDelay();
		}
		else
		{
			SpawnBuildGhost();

			BuildDelay();
		}
	}
}

void UBuildComponent::ClearBuildGhostMesh()
{
	bIsBuildMode = false;
	bCanBuild = false;

	if (IsValid(BuildGhostMeshComp))
	{
		K2_DestroyComponent(BuildGhostMeshComp);
	}
}

void UBuildComponent::BuildDelay()
{
	if (bIsBuildMode)
	{
		FTimerHandle TimerHandle;
		PlayerCharacter->GetWorldTimerManager().SetTimer(TimerHandle, this, &UBuildComponent::BuildCycle, 0.01f, false);
	}
	else
	{
		ClearBuildGhostMesh();
	}
}

void UBuildComponent::LaunchBuildMode()
{
	if (bIsBuildMode)
	{
		StopBuildMode();
	}
	else
	{
		bIsBuildMode = true;
		BuildCycle();
	}
}

void UBuildComponent::StopBuildMode()
{
	ClearBuildGhostMesh();
}

