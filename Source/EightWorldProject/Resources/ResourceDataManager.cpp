// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceDataManager.h"
#include "Engine/DataTable.h"

// 싱글톤 인스턴스 초기화
UResourceDataManager* UResourceDataManager::Instance = nullptr;

UResourceDataManager::UResourceDataManager()
{
    // 데이터 테이블 초기화
    ResourceDataTable = nullptr;
    ItemDataTable = nullptr;
}

// 자원 데이터 테이블 로드
bool UResourceDataManager::LoadResourceDataTable(const FString& Path)
{
    // 데이터 테이블 로드
    ResourceDataTable = LoadObject<UDataTable>(nullptr, *Path);
    
    // 로드 성공 여부 반환
    return (ResourceDataTable != nullptr);
}

// 아이템 데이터 테이블 로드
bool UResourceDataManager::LoadItemDataTable(const FString& Path)
{
    // 데이터 테이블 로드
    ItemDataTable = LoadObject<UDataTable>(nullptr, *Path);
    
    // 로드 성공 여부 반환
    return (ItemDataTable != nullptr);
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
        return nullptr;
    }
    
    // 아이템 ID로 데이터 검색
    return ItemDataTable->FindRow<FItemData>(ItemID, TEXT(""));
}

// 싱글톤 인스턴스 가져오기
UResourceDataManager* UResourceDataManager::GetInstance()
{
    // 인스턴스가 없으면 생성
    if (!Instance)
    {
        Instance = NewObject<UResourceDataManager>();
        
        // 기본 데이터 테이블 경로 설정 및 로드
        Instance->LoadResourceDataTable(TEXT("/Game/Data/DT_ResourceData"));
        Instance->LoadItemDataTable(TEXT("/Game/Data/DT_ItemData"));
        
        // GC에서 제외
        Instance->AddToRoot();
    }
    
    return Instance;
}
