#include "BuildComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "EightWorldProject/Interface/BuildInterface.h"
#include "EightWorldProject/Player/PlayerCharacter.h"
#include "EightWorldProject/UI/BuildModeUI.h"
#include "EightWorldProject/UI/MainUI.h"
#include "Net/UnrealNetwork.h"

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

	SetIsReplicated(true);
}


// Called when the game starts
void UBuildComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (BuildDataTable)
	{
		static const FString ContextString(TEXT("BuildDataTableContext"));
		
		TArray<FBuildData*> OutRows;
		BuildDataTable->GetAllRows<FBuildData>(ContextString, OutRows);

		for (const FBuildData* Row : OutRows)
		{
			if (Row)
			{
				BuildData.Add(*Row);
			}
		}
	}
}


// Called every frame
void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBuildComponent::SpawnBuildGhost()
{
	BuildGhostMeshComp = Cast<UStaticMeshComponent>(PlayerCharacter->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, BuildTransform, false));

	BuildGhostMeshComp->SetStaticMesh(BuildData[BuildID].Mesh);

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

			// TODO :: 인벤토리에 아이템 부족한것 표시
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
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, BuildData[BuildID].TraceChannel, Params);

	// 인벤토리 자원 체크
	int32 TreeNeed = BuildData[BuildID].TreeCost;
	int32 RockNeed = BuildData[BuildID].RockCost;
	int32 TreeHave = 0;
	int32 RockHave = 0;
	bool bEnough = true;

	if (PlayerCharacter && PlayerCharacter->InventoryComponent)
	{
		TreeHave = PlayerCharacter->InventoryComponent->GetItemQuantity(TEXT("Tree"));
		RockHave = PlayerCharacter->InventoryComponent->GetItemQuantity(TEXT("Rock"));
		bEnough = (TreeHave >= TreeNeed) && (RockHave >= RockNeed);
	}

	// 빌드 고스트 색상
	GiveBuildColor(bEnough);

	// UI 갱신
	if (PlayerCharacter && PlayerCharacter->MainUI && PlayerCharacter->MainUI->BuildModeUI)
	{
		PlayerCharacter->MainUI->BuildModeUI->SetBuildCost(TreeHave, TreeNeed, RockHave, RockNeed, bEnough);
	}

	if (bHit)
	{
		// 충돌 위치 저장
		BuildTransform.SetLocation(hitResult.ImpactPoint);
		// 충돌 Actor, Component 저장
		HitActor = hitResult.GetActor();
		HitComp = Cast<UBoxComponent>(hitResult.GetComponent());
		
		if (BuildGhostMeshComp)
		{
			// 스냅 가능한지 여부
			DetectBuildBoxes();
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
		BuildGhostMeshComp->DestroyComponent();
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
		if (PlayerCharacter && PlayerCharacter->MainUI)
		{
			PlayerCharacter->MainUI->SetBuildModeUIVisible(true);
		}
		SpawnBuildGhost();
		BuildCycle();
	}
}

void UBuildComponent::StopBuildMode()
{
	if (PlayerCharacter && PlayerCharacter->MainUI)
	{
		PlayerCharacter->MainUI->SetBuildModeUIVisible(false);
	}
	ClearBuildGhostMesh();
}

void UBuildComponent::ChangeMesh()
{
	if (IsValid(BuildGhostMeshComp))
	{
		BuildGhostMeshComp->SetStaticMesh(BuildData[BuildID].Mesh);
	}
}

void UBuildComponent::SpawnBuild()
{
	GetWorld()->SpawnActor<AActor>(BuildData[BuildID].Actor, BuildTransform);
}

void UBuildComponent::DetectBuildBoxes()
{
	if (IBuildInterface* BuildInterface = Cast<IBuildInterface>(HitActor))
	{
		TArray<UBoxComponent*> Boxes;
		Boxes.Empty();
		Boxes = BuildInterface->ReturnBoxes();

		for (UBoxComponent* Box : Boxes)
		{
			if (Box == HitComp)
			{
				// 감지 되었을때
				BuildTransform = HitComp->GetComponentTransform();
				break;
			}
		}
	}
	
}

void UBuildComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBuildComponent, bIsBuildMode);
	DOREPLIFETIME(UBuildComponent, bCanBuild);

}

