#pragma once

#include "CoreMinimal.h"
#include "InventoryComponent.h"
#include "InventoryDragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "InventoryWidget.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// 인벤토리 초기화
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory(UInventoryComponent* InInventoryComponent);
	
	// 인벤토리 업데이트
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateInventory();
	
	// 슬롯 클릭 처리
	UFUNCTION()
	void HandleSlotClicked(int32 SlotIndex);
	
	// 슬롯 호버 처리
	UFUNCTION()
	void HandleSlotHovered(int32 SlotIndex);
	
	// 슬롯 언호버 처리
	UFUNCTION()
	void HandleSlotUnhovered();
	
	// 드래그 드롭 완료 처리
	UFUNCTION()
	void HandleDragDropCompleted(UInventoryDragDropOperation* Operation);
	
	// 인벤토리 컴포넌트
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	class UInventoryComponent* InventoryComponent;
	
	// 인벤토리 그리드 패널
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;
	
	// 툴팁 캔버스
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* TooltipCanvas;
	
	// 슬롯 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<class UInventorySlotWidget> SlotWidgetClass;
	
	// 툴팁 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSubclassOf<class UItemTooltipWidget> TooltipWidgetClass;
	
	// 인벤토리 슬롯 배열
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TArray<class UInventorySlotWidget*> SlotWidgets;
	
	// 툴팁 아이템 위젯
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	class UItemTooltipWidget* ItemTooltipWidget;
	
	// 인벤토리 행 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 GridRows = 6;
	
	// 인벤토리 열 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 GridColumns = 7;
};
