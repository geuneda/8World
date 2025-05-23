#include "TreeItem.h"
#include "Components/StaticMeshComponent.h"

ATreeItem::ATreeItem()
{
    // 리소스 ID 설정
    ResourceID = TEXT("Tree");
    
    // 나무 아이템 메시 설정 
    static ConstructorHelpers::FObjectFinder<UStaticMesh> TreeItemMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Items/TreeItem.TreeItem'"));
    if (TreeItemMesh.Succeeded() && Mesh)
    {
        Mesh->SetStaticMesh(TreeItemMesh.Object);
    }
}

void ATreeItem::Initialize()
{
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
}
