#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "ItemTooltipWidget.h"
#include "InventoryDragDropOperation.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!ItemTooltipWidget && TooltipWidgetClass)
	{
		ItemTooltipWidget = CreateWidget<UItemTooltipWidget>(this, TooltipWidgetClass);
		if (ItemTooltipWidget && TooltipCanvas)
		{
			TooltipCanvas->AddChild(ItemTooltipWidget);
			ItemTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// 툴팁 위치 업데이트
	if (ItemTooltipWidget && ItemTooltipWidget->IsVisible())
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		MousePosition = MousePosition / Scale;
		
		// 툴팁이 화면 밖으로 나가지 않도록 위치 조정
		FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
		FVector2D TooltipSize = ItemTooltipWidget->GetDesiredSize();
		
		float OffsetX = 15.0f;
		float OffsetY = 15.0f;
		
		if (MousePosition.X + TooltipSize.X + OffsetX > ViewportSize.X)
		{
			OffsetX = -TooltipSize.X - 15.0f;
		}
		
		if (MousePosition.Y + TooltipSize.Y + OffsetY > ViewportSize.Y)
		{
			OffsetY = -TooltipSize.Y - 15.0f;
		}
		
		ItemTooltipWidget->SetRenderTranslation(FVector2D(MousePosition.X + OffsetX, MousePosition.Y + OffsetY));
	}
}

void UInventoryWidget::InitializeInventory(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	
	if (!InventoryComponent || !InventoryGrid || !SlotWidgetClass)
	{
		return;
	}
	
	// 기존 슬롯 제거
	InventoryGrid->ClearChildren();
	SlotWidgets.Empty();
	
	// 슬롯 생성
	for (int32 i = 0; i < InventoryComponent->MaxSlots; i++)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		if (SlotWidget)
		{
			SlotWidget->SlotIndex = i;
			SlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryWidget::HandleSlotClicked);
			SlotWidget->OnSlotHovered.AddDynamic(this, &UInventoryWidget::HandleSlotHovered);
			SlotWidget->OnSlotUnhovered.AddDynamic(this, &UInventoryWidget::HandleSlotUnhovered);
			
			int32 Row = i / GridColumns;
			int32 Column = i % GridColumns;
			
			InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
			SlotWidgets.Add(SlotWidget);
		}
	}
	
	// 인벤토리 업데이트 이벤트 바인딩
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::UpdateInventory);
		UpdateInventory();
	}
}

void UInventoryWidget::UpdateInventory()
{
	if (!InventoryComponent)
	{
		return;
	}
	
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		if (SlotWidgets[i])
		{
			FInventorySlotData SlotData = InventoryComponent->GetSlotData(i);
			SlotWidgets[i]->UpdateSlot(SlotData);
		}
	}
}

void UInventoryWidget::HandleSlotClicked(int32 SlotIndex)
{
	// TODO :: 필요 시 슬롯 클릭시 행동 구현
}

void UInventoryWidget::HandleSlotHovered(int32 SlotIndex)
{
	if (!InventoryComponent || !ItemTooltipWidget)
	{
		return;
	}
	
	FInventorySlotData SlotData = InventoryComponent->GetSlotData(SlotIndex);
	if (!SlotData.IsEmpty())
	{
		ItemTooltipWidget->UpdateTooltip(SlotData.ItemData);
		ItemTooltipWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventoryWidget::HandleSlotUnhovered()
{
	if (ItemTooltipWidget)
	{
		ItemTooltipWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::HandleDragDropCompleted(UInventoryDragDropOperation* Operation)
{
	if (!InventoryComponent || !Operation)
	{
		return;
	}
	
	int32 FromSlotIndex = Operation->SourceSlotIndex;
	int32 ToSlotIndex = Operation->TargetSlotIndex;
	
	if (FromSlotIndex != ToSlotIndex)
	{
		InventoryComponent->MoveItem(FromSlotIndex, ToSlotIndex);
	}
}
