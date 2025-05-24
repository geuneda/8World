#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryTypes.h"
#include "InventoryDragDropOperation.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	// 드래그 시작 슬롯 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SourceSlotIndex;
	
	// 드래그 타겟 슬롯 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 TargetSlotIndex;
	
	// 슬롯 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FInventorySlotData SlotData;
};
