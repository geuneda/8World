#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventorySlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotHovered, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotUnhovered);

UCLASS()
class EIGHTWORLDPROJECT_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// 슬롯 데이터 업데이트
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateSlot(const FInventorySlotData& InSlotData);

	// 슬롯 비우기
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearSlot();

	// 슬롯 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SlotIndex;

	// 슬롯 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FInventorySlotData SlotData;

	// 아이템 아이콘
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemIcon;

	// 아이템 수량 텍스트
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* QuantityText;

	// 슬롯 배경
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotBackground;

	// 슬롯 클릭 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotClicked OnSlotClicked;

	// 슬롯 호버 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotHovered OnSlotHovered;

	// 슬롯 언호버 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotUnhovered OnSlotUnhovered;
};
