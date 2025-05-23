#include "RockItem.h"
#include "Components/StaticMeshComponent.h"

ARockItem::ARockItem()
{
    // 리소스 ID 설정
    ResourceID = TEXT("Rock");
    
    // 돌 아이템 메시 설정 - 생성자에서 메시 로드
    static ConstructorHelpers::FObjectFinder<UStaticMesh> RockItemMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Items/RockItem.RockItem'"));
    if (RockItemMesh.Succeeded() && Mesh)
    {
        Mesh->SetStaticMesh(RockItemMesh.Object);
    }
}

void ARockItem::Initialize()
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
