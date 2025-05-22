// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceDataTypes.h"
#include "ResourceDataManager.generated.h"

/**
 * 자원 데이터 관리자 클래스
 * 데이터 테이블에서 자원 및 아이템 정보를 로드하고 관리
 */
UCLASS()
class EIGHTWORLDPROJECT_API UResourceDataManager : public UObject
{
	GENERATED_BODY()
	
public:
	UResourceDataManager();

	// 자원 데이터 테이블 로드
	bool LoadResourceDataTable(const FString& Path);

	// 아이템 데이터 테이블 로드
	bool LoadItemDataTable(const FString& Path);

	// 자원 데이터 가져오기
	FResourceData* GetResourceData(FName ResourceID);

	// 아이템 데이터 가져오기
	FItemData* GetItemData(FName ItemID);

	// 싱글톤 인스턴스 가져오기
	static UResourceDataManager* GetInstance();

private:
	// 자원 데이터 테이블
	UPROPERTY()
	class UDataTable* ResourceDataTable;

	// 아이템 데이터 테이블
	UPROPERTY()
	class UDataTable* ItemDataTable;

	// 싱글톤 인스턴스
	static UResourceDataManager* Instance;
};
