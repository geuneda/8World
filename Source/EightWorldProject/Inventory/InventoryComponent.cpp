#include "InventoryComponent.h"
#include "../Resources/ResourceDataManager.h"
#include "../Public/PWGameMode.h"
#include "Kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ResourceDataManager = GetResourceDataManager();
	InitializeInventory();
}

UResourceDataManager* UInventoryComponent::GetResourceDataManager()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	APWGameMode* GameMode = Cast<APWGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode)
	{
		return nullptr;
	}
	
	return GameMode->GetResourceDataManager();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::InitializeInventory()
{
	InventorySlots.Empty();
	
	for (int32 i = 0; i < MaxSlots; i++)
	{
		InventorySlots.Add(FInventorySlotData(i));
	}
}

bool UInventoryComponent::AddItem(FName ItemID, int32 Quantity)
{
	if (ItemID.IsNone() || Quantity <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AddItem: Invalid ItemID or Quantity: %s, %d"), *ItemID.ToString(), Quantity);
		return false;
	}
	
	// ResourceDataManager가 null이면 다시 가져오기 시도
	if (!ResourceDataManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddItem: ResourceDataManager is null, trying to get it again"));
		ResourceDataManager = GetResourceDataManager();
		
		if (!ResourceDataManager)
		{
			UE_LOG(LogTemp, Error, TEXT("AddItem: Failed to get ResourceDataManager"));
			return false;
		}
	}

	// 디버깅: 아이템 데이터 테이블 상태 확인
	UE_LOG(LogTemp, Warning, TEXT("AddItem: Trying to get item data for ID: %s"), *ItemID.ToString());
	
	// 아이템 데이터 테이블 유효성 확인
	if (!ResourceDataManager->ItemDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("AddItem: ItemDataTable is null"));
		return false;
	}
	
	FItemData* ItemData = ResourceDataManager->GetItemData(ItemID);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("AddItem: Failed to find ItemData for ID: %s"), *ItemID.ToString());
		return false;
	}

	int32 RemainingQuantity = Quantity;
	
	// 1. 기존 스택에 추가 시도
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (RemainingQuantity <= 0)
		{
			break;
		}

		if (!InventorySlots[i].IsEmpty() && InventorySlots[i].ItemID == ItemID)
		{
			int32 CurrentQuantity = InventorySlots[i].Quantity;
			int32 MaxStackSize = ItemData->MaxStackCount;
			int32 CanAddQuantity = FMath::Min(RemainingQuantity, MaxStackSize - CurrentQuantity);
			
			if (CanAddQuantity > 0)
			{
				InventorySlots[i].Quantity += CanAddQuantity;
				InventorySlots[i].ItemData = *ItemData;
				RemainingQuantity -= CanAddQuantity;
			}
		}
	}
	
	// 2. 빈 슬롯에 추가
	while (RemainingQuantity > 0)
	{
		int32 EmptySlot = FindEmptySlot();
		if (EmptySlot == -1)
		{
			// 인벤토리가 가득 참
			OnInventoryUpdated.Broadcast();
			OnItemAdded.Broadcast(ItemID, Quantity - RemainingQuantity);
			return RemainingQuantity < Quantity; // 일부만 추가되었어도 true 반환
		}
		
		int32 MaxStackSize = ItemData->MaxStackCount;
		int32 StackQuantity = FMath::Min(RemainingQuantity, MaxStackSize);
		
		InventorySlots[EmptySlot].ItemID = ItemID;
		InventorySlots[EmptySlot].Quantity = StackQuantity;
		InventorySlots[EmptySlot].ItemData = *ItemData;
		
		RemainingQuantity -= StackQuantity;
	}
	
	OnInventoryUpdated.Broadcast();
	OnItemAdded.Broadcast(ItemID, Quantity);
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	if (ItemID.IsNone() || Quantity <= 0)
	{
		return false;
	}
	
	int32 TotalQuantity = GetItemQuantity(ItemID);
	if (TotalQuantity < Quantity)
	{
		return false;
	}
	
	int32 RemainingToRemove = Quantity;
	
	// 아이템 제거 (뒤에서부터 제거하여 UI 업데이트 최소화)
	for (int32 i = InventorySlots.Num() - 1; i >= 0; i--)
	{
		if (RemainingToRemove <= 0)
		{
			break;
		}
		
		if (!InventorySlots[i].IsEmpty() && InventorySlots[i].ItemID == ItemID)
		{
			int32 SlotQuantity = InventorySlots[i].Quantity;
			int32 RemoveAmount = FMath::Min(RemainingToRemove, SlotQuantity);
			
			InventorySlots[i].Quantity -= RemoveAmount;
			RemainingToRemove -= RemoveAmount;
			
			if (InventorySlots[i].Quantity <= 0)
			{
				InventorySlots[i].ItemID = NAME_None;
				InventorySlots[i].Quantity = 0;
			}
		}
	}
	
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItemFromSlot(int32 SlotIndex, int32 Quantity)
{
	if (SlotIndex < 0 || SlotIndex >= InventorySlots.Num() || Quantity <= 0)
	{
		return false;
	}
	
	if (InventorySlots[SlotIndex].IsEmpty() || InventorySlots[SlotIndex].Quantity < Quantity)
	{
		return false;
	}
	
	InventorySlots[SlotIndex].Quantity -= Quantity;
	
	if (InventorySlots[SlotIndex].Quantity <= 0)
	{
		InventorySlots[SlotIndex].ItemID = NAME_None;
		InventorySlots[SlotIndex].Quantity = 0;
	}
	
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UInventoryComponent::MoveItem(int32 FromSlotIndex, int32 ToSlotIndex)
{
	if (FromSlotIndex < 0 || FromSlotIndex >= InventorySlots.Num() ||
		ToSlotIndex < 0 || ToSlotIndex >= InventorySlots.Num() ||
		FromSlotIndex == ToSlotIndex)
	{
		return false;
	}
	
	if (InventorySlots[FromSlotIndex].IsEmpty())
	{
		return false;
	}
	
	// 대상 슬롯이 비어있는 경우
	if (InventorySlots[ToSlotIndex].IsEmpty())
	{
		InventorySlots[ToSlotIndex] = InventorySlots[FromSlotIndex];
		InventorySlots[ToSlotIndex].SlotIndex = ToSlotIndex;
		
		InventorySlots[FromSlotIndex] = FInventorySlotData(FromSlotIndex);
		
		OnInventoryUpdated.Broadcast();
		return true;
	}
	
	// 같은 아이템이고 스택 가능한 경우
	if (InventorySlots[FromSlotIndex].ItemID == InventorySlots[ToSlotIndex].ItemID)
	{
		FItemData* ItemData = ResourceDataManager->GetItemData(InventorySlots[FromSlotIndex].ItemID);
		if (ItemData)
		{
			int32 MaxStackSize = ItemData->MaxStackCount;
			int32 ToSlotQuantity = InventorySlots[ToSlotIndex].Quantity;
			int32 FromSlotQuantity = InventorySlots[FromSlotIndex].Quantity;
			
			if (ToSlotQuantity < MaxStackSize)
			{
				int32 CanAddQuantity = FMath::Min(FromSlotQuantity, MaxStackSize - ToSlotQuantity);
				
				InventorySlots[ToSlotIndex].Quantity += CanAddQuantity;
				InventorySlots[FromSlotIndex].Quantity -= CanAddQuantity;
				
				if (InventorySlots[FromSlotIndex].Quantity <= 0)
				{
					InventorySlots[FromSlotIndex] = FInventorySlotData(FromSlotIndex);
				}
				
				OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}
	
	// 그 외의 경우 스왑
	return SwapItems(FromSlotIndex, ToSlotIndex);
}

bool UInventoryComponent::SwapItems(int32 SlotIndex1, int32 SlotIndex2)
{
	if (SlotIndex1 < 0 || SlotIndex1 >= InventorySlots.Num() ||
		SlotIndex2 < 0 || SlotIndex2 >= InventorySlots.Num() ||
		SlotIndex1 == SlotIndex2)
	{
		return false;
	}
	
	FInventorySlotData TempSlot = InventorySlots[SlotIndex1];
	
	InventorySlots[SlotIndex1] = InventorySlots[SlotIndex2];
	InventorySlots[SlotIndex1].SlotIndex = SlotIndex1;
	
	InventorySlots[SlotIndex2] = TempSlot;
	InventorySlots[SlotIndex2].SlotIndex = SlotIndex2;
	
	OnInventoryUpdated.Broadcast();
	return true;
}

FInventorySlotData UInventoryComponent::GetSlotData(int32 SlotIndex) const
{
	if (SlotIndex >= 0 && SlotIndex < InventorySlots.Num())
	{
		return InventorySlots[SlotIndex];
	}
	
	return FInventorySlotData();
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return 0;
	}
	
	int32 TotalQuantity = 0;
	
	for (const FInventorySlotData& Slot : InventorySlots)
	{
		if (!Slot.IsEmpty() && Slot.ItemID == ItemID)
		{
			TotalQuantity += Slot.Quantity;
		}
	}
	
	return TotalQuantity;
}

int32 UInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].IsEmpty())
		{
			return i;
		}
	}
	
	return -1;
}

int32 UInventoryComponent::FindItemSlot(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return -1;
	}
	
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (!InventorySlots[i].IsEmpty() && InventorySlots[i].ItemID == ItemID)
		{
			return i;
		}
	}
	
	return -1;
}

bool UInventoryComponent::CanStackItem(FName ItemID, int32 SlotIndex) const
{
	if (ItemID.IsNone() || SlotIndex < 0 || SlotIndex >= InventorySlots.Num())
	{
		return false;
	}
	
	const FInventorySlotData& Slot = InventorySlots[SlotIndex];
	
	if (Slot.IsEmpty())
	{
		return true;
	}
	
	if (Slot.ItemID != ItemID)
	{
		return false;
	}
	
	FItemData* ItemData = ResourceDataManager->GetItemData(ItemID);
	if (!ItemData)
	{
		return false;
	}
	
	return Slot.Quantity < ItemData->MaxStackCount;
}
