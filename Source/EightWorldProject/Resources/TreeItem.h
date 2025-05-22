#pragma once

#include "CoreMinimal.h"
#include "ResourceItem.h"
#include "TreeItem.generated.h"

/**
 * 나무 아이템 클래스
 * 나무에서 드롭되는 아이템
 */
UCLASS()
class EIGHTWORLDPROJECT_API ATreeItem : public AResourceItem
{
	GENERATED_BODY()
	
public:	
	ATreeItem();

	// 아이템 초기화 재정의
	virtual void Initialize() override;
};
