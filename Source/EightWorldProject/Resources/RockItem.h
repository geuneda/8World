#pragma once

#include "CoreMinimal.h"
#include "ResourceItem.h"
#include "RockItem.generated.h"

/**
 * 돌 아이템 클래스
 * 돌에서 드롭되는 아이템
 */
UCLASS()
class EIGHTWORLDPROJECT_API ARockItem : public AResourceItem
{
	GENERATED_BODY()
	
public:	
	ARockItem();

	// 아이템 초기화 재정의
	virtual void Initialize() override;
};
