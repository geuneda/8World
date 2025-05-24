// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceDataTypes.h"
#include "ResourceDataManager.generated.h"

/**
 * 자원 데이터 관리자 클래스
 * 데이터 테이블에서 자원 및 아이템 정보를 로드하고 관리
 */
UCLASS(Blueprintable)
class EIGHTWORLDPROJECT_API UResourceDataManager : public UObject
{
	GENERATED_BODY()
	
public:
	UResourceDataManager();

	// 초기화 함수
	UFUNCTION(BlueprintCallable, Category = "Resource Data")
	void Initialize();

	// 자원 데이터 가져오기
	FResourceData* GetResourceData(FName ResourceID);

	// 아이템 데이터 가져오기
	FItemData* GetItemData(FName ItemID);

	// 데이터 테이블 직접 설정 (블루프린트에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Resource Data")
	void SetResourceDataTable(UDataTable* InResourceDataTable);

	// 데이터 테이블 직접 설정 (블루프린트에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Resource Data")
	void SetItemDataTable(UDataTable* InItemDataTable);

public:
	// 자원 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
	class UDataTable* ResourceDataTable;

	// 아이템 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
	class UDataTable* ItemDataTable;
};
