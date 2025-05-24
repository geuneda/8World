// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Resources/ResourceDataTypes.h"
#include "InventoryTypes.generated.h"

/**
 * 인벤토리 슬롯 데이터 구조체
 * 인벤토리 슬롯의 아이템 정보를 저장
 */
USTRUCT(BlueprintType)
struct EIGHTWORLDPROJECT_API FInventorySlotData
{
    GENERATED_USTRUCT_BODY()

public:
    // 슬롯 인덱스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SlotIndex;

    // 아이템 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FName ItemID;

    // 아이템 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Quantity;

    // 아이템 데이터 캐시
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FItemData ItemData;

    // 슬롯이 비어있는지 확인
    bool IsEmpty() const
    {
        return ItemID.IsNone() || Quantity <= 0;
    }

    // 기본 생성자
    FInventorySlotData()
        : SlotIndex(-1)
        , ItemID(NAME_None)
        , Quantity(0)
    {
    }

    // 인덱스만 지정하는 생성자
    FInventorySlotData(int32 InSlotIndex)
        : SlotIndex(InSlotIndex)
        , ItemID(NAME_None)
        , Quantity(0)
    {
    }

    // 모든 값을 지정하는 생성자
    FInventorySlotData(int32 InSlotIndex, FName InItemID, int32 InQuantity)
        : SlotIndex(InSlotIndex)
        , ItemID(InItemID)
        , Quantity(InQuantity)
    {
    }
};
