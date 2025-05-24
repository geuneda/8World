#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "../Resources/ResourceDataManager.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, FName, ItemID, int32, Quantity);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class EIGHTWORLDPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 인벤토리 슬롯 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlotData> InventorySlots;

	// 인벤토리 최대 슬롯 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots = 42;

	// 인벤토리 업데이트 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	// 아이템 추가 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	// 인벤토리 초기화
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory();

	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemID, int32 Quantity);

	// 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity);

	// 슬롯에서 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemFromSlot(int32 SlotIndex, int32 Quantity);

	// 슬롯 아이템 이동
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(int32 FromSlotIndex, int32 ToSlotIndex);

	// 슬롯 아이템 스왑
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItems(int32 SlotIndex1, int32 SlotIndex2);

	// 슬롯 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventorySlotData GetSlotData(int32 SlotIndex) const;

	// 아이템 수량 확인
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantity(FName ItemID) const;

	// 빈 슬롯 인덱스 찾기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 FindEmptySlot() const;

	// 아이템이 있는 슬롯 인덱스 찾기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 FindItemSlot(FName ItemID) const;

	// 아이템 스택 가능 여부 확인
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanStackItem(FName ItemID, int32 SlotIndex) const;

private:
	// 리소스 데이터 관리자 참조
	UPROPERTY()
	UResourceDataManager* ResourceDataManager;
	
	// 리소스 데이터 관리자 가져오기
	UResourceDataManager* GetResourceDataManager();
};
