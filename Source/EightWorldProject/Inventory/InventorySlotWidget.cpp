#include "InventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InventoryDragDropOperation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EightWorldProject/Resources/ResourceDataTypes.h"
#include "Engine/Texture2D.h"
#include "InventoryWidget.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnSlotClicked.Broadcast(SlotIndex);
		
		if (!SlotData.IsEmpty())
		{
			return Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
		}
	}
	
	return Reply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	if (SlotData.IsEmpty())
	{
		return;
	}
	
	UInventoryDragDropOperation* DragDropOp = Cast<UInventoryDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventoryDragDropOperation::StaticClass()));
	if (DragDropOp)
	{
		UInventorySlotWidget* DragVisual = CreateWidget<UInventorySlotWidget>(this, GetClass());
		if (DragVisual)
		{
			DragVisual->SlotIndex = SlotIndex;
			DragVisual->SlotData = SlotData;
			DragVisual->UpdateSlot(SlotData);
			
			DragDropOp->DefaultDragVisual = DragVisual;
			DragDropOp->SourceSlotIndex = SlotIndex;
			DragDropOp->SlotData = SlotData;
			DragDropOp->Pivot = EDragPivot::CenterCenter;
		}
		
		OutOperation = DragDropOp;
	}
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UInventoryDragDropOperation* InventoryDragDrop = Cast<UInventoryDragDropOperation>(InOperation);
	if (InventoryDragDrop)
	{
		InventoryDragDrop->TargetSlotIndex = SlotIndex;
		
		// 소유 플레이어의 인벤토리 컴포넌트를 통해 아이템 이동
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				UInventoryComponent* InventoryComp = Pawn->FindComponentByClass<UInventoryComponent>();
				if (InventoryComp)
				{
					// 아이템 이동 처리
					int32 FromSlotIndex = InventoryDragDrop->SourceSlotIndex;
					int32 ToSlotIndex = SlotIndex;
					
					if (FromSlotIndex != ToSlotIndex)
					{
						// 아이템 이동 실행
						InventoryComp->MoveItem(FromSlotIndex, ToSlotIndex);
						
						// 인벤토리 컴포넌트의 이벤트를 통해 UI 업데이트
						InventoryComp->OnInventoryUpdated.Broadcast();
					}
				}
			}
		}
		
		return true;
	}
	
	return false;
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (!SlotData.IsEmpty())
	{
		OnSlotHovered.Broadcast(SlotIndex);
	}
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	OnSlotUnhovered.Broadcast();
}

void UInventorySlotWidget::UpdateSlot(const FInventorySlotData& InSlotData)
{
	SlotData = InSlotData;
	
	if (SlotData.IsEmpty())
	{
		ClearSlot();
		return;
	}
	
	if (ItemIcon)
	{
		if (!SlotData.ItemData.ItemIconPath.IsEmpty())
		{
			UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *SlotData.ItemData.ItemIconPath);
			if (IconTexture)
			{
				ItemIcon->SetBrushFromTexture(IconTexture);
				ItemIcon->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	
	if (QuantityText)
	{
		if (SlotData.Quantity > 1)
		{
			QuantityText->SetText(FText::AsNumber(SlotData.Quantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UInventorySlotWidget::ClearSlot()
{
	SlotData = FInventorySlotData(SlotIndex);
	
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (QuantityText)
	{
		QuantityText->SetVisibility(ESlateVisibility::Hidden);
	}
}
