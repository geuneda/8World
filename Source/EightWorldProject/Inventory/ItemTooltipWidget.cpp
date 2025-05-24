#include "ItemTooltipWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UItemTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemTooltipWidget::UpdateTooltip(const FItemData& InItemData)
{
	if (ItemNameText)
	{
		ItemNameText->SetText(InItemData.ItemName);
	}
	
	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(InItemData.ItemDescription);
	}
	
	if (ItemIcon && !InItemData.ItemIconPath.IsEmpty())
	{
		UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *InItemData.ItemIconPath);
		if (IconTexture)
		{
			ItemIcon->SetBrushFromTexture(IconTexture);
		}
	}
}
