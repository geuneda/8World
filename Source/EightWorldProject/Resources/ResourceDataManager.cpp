// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceDataManager.h"
#include "Engine/DataTable.h"

UResourceDataManager::UResourceDataManager()
{
    // 데이터 테이블 초기화
    ResourceDataTable = nullptr;
    ItemDataTable = nullptr;
}

// 초기화 함수
void UResourceDataManager::Initialize()
{
    // 데이터 테이블 상태 확인 및 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: Initialize called"));
    
    if (ResourceDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: ResourceDataTable is valid"));
        
        // 데이터 테이블의 로우 이름 출력 (처음 10개만)
        TArray<FName> ResourceRowNames = ResourceDataTable->GetRowNames();
        int32 Count = FMath::Min(ResourceRowNames.Num(), 10);
        FString RowNamesStr;
        for (int32 i = 0; i < Count; i++)
        {
            RowNamesStr += ResourceRowNames[i].ToString();
            if (i < Count - 1) RowNamesStr += TEXT(", ");
        }
        UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: Available rows in ResourceDataTable (up to 10): %s"), *RowNamesStr);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ResourceDataManager: ResourceDataTable is null"));
    }
    
    if (ItemDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: ItemDataTable is valid"));
        
        // 데이터 테이블의 로우 이름 출력 (처음 10개만)
        TArray<FName> ItemRowNames = ItemDataTable->GetRowNames();
        int32 Count = FMath::Min(ItemRowNames.Num(), 10);
        FString RowNamesStr;
        for (int32 i = 0; i < Count; i++)
        {
            RowNamesStr += ItemRowNames[i].ToString();
            if (i < Count - 1) RowNamesStr += TEXT(", ");
        }
        UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: Available rows in ItemDataTable (up to 10): %s"), *RowNamesStr);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ResourceDataManager: ItemDataTable is null"));
    }
}

// 데이터 테이블 설정 함수
void UResourceDataManager::SetResourceDataTable(UDataTable* InResourceDataTable)
{
    ResourceDataTable = InResourceDataTable;
    UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: ResourceDataTable set to %s"), ResourceDataTable ? TEXT("valid reference") : TEXT("null"));
}

void UResourceDataManager::SetItemDataTable(UDataTable* InItemDataTable)
{
    ItemDataTable = InItemDataTable;
    UE_LOG(LogTemp, Warning, TEXT("ResourceDataManager: ItemDataTable set to %s"), ItemDataTable ? TEXT("valid reference") : TEXT("null"));
}

// 자원 데이터 가져오기
FResourceData* UResourceDataManager::GetResourceData(FName ResourceID)
{
    // 데이터 테이블이 로드되었는지 확인
    if (!ResourceDataTable)
    {
        return nullptr;
    }
    
    // 자원 ID로 데이터 검색
    return ResourceDataTable->FindRow<FResourceData>(ResourceID, TEXT(""));
}

// 아이템 데이터 가져오기
FItemData* UResourceDataManager::GetItemData(FName ItemID)
{
    // 데이터 테이블이 로드되었는지 확인
    if (!ItemDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("GetItemData: ItemDataTable is null"));
        return nullptr;
    }
    
    // 디버깅: 아이템 ID 출력
    UE_LOG(LogTemp, Warning, TEXT("GetItemData: Searching for ItemID: %s"), *ItemID.ToString());
    
    // 데이터 테이블의 로우 이름 출력 (처음 10개만)
    TArray<FName> RowNames = ItemDataTable->GetRowNames();
    int32 Count = FMath::Min(RowNames.Num(), 10);
    FString RowNamesStr;
    for (int32 i = 0; i < Count; i++)
    {
        RowNamesStr += RowNames[i].ToString();
        if (i < Count - 1) RowNamesStr += TEXT(", ");
    }
    UE_LOG(LogTemp, Warning, TEXT("GetItemData: Available rows in ItemDataTable (up to 10): %s"), *RowNamesStr);
    
    // 아이템 ID로 데이터 검색
    FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemID, TEXT(""));
    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("GetItemData: Failed to find row with ID: %s"), *ItemID.ToString());
    }
    
    return ItemData;
}


