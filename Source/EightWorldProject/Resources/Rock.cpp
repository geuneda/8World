#include "Rock.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "ResourceManager.h"

ARock::ARock()
{
	// 충돌 컴포넌트 크기 설정
	Box->SetBoxExtent(FVector(140, 140, 160));
	
	// 리소스 ID 설정
	ResourceID = TEXT("Rock");

	// 태그 추가
	Tags.Add(FName("Rock"));
	
	// 리소스 메시 로드
	LoadResourceMesh();
}

void ARock::HandleDeactivation()
{
	// ResourceManager에 비활성화 요청
	AResourceManager* ResourceManager = GetResourceManager();
	if (ResourceManager)
	{
		ResourceManager->DeactivateRock(this);
		//ResourceManager->SetReplicates(true);
		//ResourceManager->SetReplicateMovement(true);
	}
}

void ARock::LoadResourceMesh()
{
	// ResourceDataManager에서 설정 값 가져오기 시도
	UResourceDataManager* DataManager = GetResourceDataManager();
	if (DataManager)
	{
		FResourceData* ResourceData = DataManager->GetResourceData(ResourceID);
		if (ResourceData)
		{
			// 데이터 테이블에서 값 가져오기
			MaxHealth = ResourceData->MaxHealth;
			MinHealth = ResourceData->MinHealth;
			ItemSpawnHealthInterval = ResourceData->ItemSpawnHealthInterval;
			
			// 에셋 경로 데이터 기반 로드
			if (!ResourceData->ResourceMeshPath.IsEmpty())
			{
				UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *ResourceData->ResourceMeshPath));
				if (LoadedMesh)
				{
					Mesh->SetStaticMesh(LoadedMesh);
				}
			}
		}
	}
	
	// 기본 메시 에셋 로드 (데이터 관리자에서 가져오지 못한 경우)
	if (!Mesh->GetStaticMesh())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Rock/Rock.Rock'"));
		if (MeshAsset.Succeeded())
		{
			Mesh->SetStaticMesh(MeshAsset.Object);
		}
	}
}