// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ResourceDataTypes.generated.h"

/**
 * 자원 데이터 구조체
 * 자원(나무, 돌)의 속성을 정의
 */
USTRUCT(BlueprintType)
struct EIGHTWORLDPROJECT_API FResourceData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // 자원 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FName ResourceID;

    // 자원 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FText ResourceName;

    // 자원 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FText ResourceDescription;

    // 자원 메시 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FString ResourceMeshPath;

    // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    float MaxHealth;

    // 최소 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    float MinHealth;

    // 아이템 스폰 체력 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    float ItemSpawnHealthInterval;

    // 충돌체 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FVector CollisionExtent;

    // 기본 생성자
    FResourceData()
        : ResourceID(NAME_None)
        , ResourceName(FText::FromString(""))
        , ResourceDescription(FText::FromString(""))
        , ResourceMeshPath("")
        , MaxHealth(100.0f)
        , MinHealth(0.0f)
        , ItemSpawnHealthInterval(30.0f)
        , CollisionExtent(FVector(50.0f, 50.0f, 50.0f))
    {
    }
};

/**
 * 아이템 데이터 구조체
 * 자원에서 드롭되는 아이템의 속성을 정의
 */
USTRUCT(BlueprintType)
struct EIGHTWORLDPROJECT_API FItemData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    // 아이템 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID;

    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    // 아이템 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemDescription;

    // 아이템 메시 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemMeshPath;

    // 아이템 아이콘 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemIconPath;

    // 아이템 최대 중첩 수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxStackCount;

    // 아이템 드롭 확률 (0.0 ~ 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float DropChance;

    // 아이템 드롭 최소 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MinDropQuantity;

    // 아이템 드롭 최대 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 MaxDropQuantity;

    // 기본 생성자
    FItemData()
        : ItemID(NAME_None)
        , ItemName(FText::FromString(""))
        , ItemDescription(FText::FromString(""))
        , ItemMeshPath("")
        , ItemIconPath("")
        , MaxStackCount(99)
        , DropChance(1.0f)
        , MinDropQuantity(1)
        , MaxDropQuantity(1)
    {
    }
};
