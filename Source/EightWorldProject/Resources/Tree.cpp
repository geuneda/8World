#include "Tree.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ResourceManager.h"

ATree::ATree()
{
	// 충돌 컴포넌트 크기 설정
	Box->SetBoxExtent(FVector(50.0f, 50.0f, 150.0f));
	
	// 리소스 ID 설정
	ResourceID = TEXT("Tree");

	// 태그 추가
	Tags.Add(FName("Tree"));
	
	// 리소스 메시 로드
	LoadResourceMesh();
}

void ATree::HandleDeactivation()
{
	// ResourceManager에 비활성화 요청
	AResourceManager* ResourceManager = GetResourceManager();
	if (ResourceManager)
	{
		ResourceManager->DeactivateTree(this);
		ResourceManager->SetReplicates(true);
		ResourceManager->SetReplicateMovement(true);
	}
}

void ATree::LoadResourceMesh()
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
			
			// 메시 에셋 경로가 있는 경우 로드
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
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Tree/Tree.Tree'"));
		if (MeshAsset.Succeeded())
		{
			Mesh->SetStaticMesh(MeshAsset.Object);
		}
	}
}
