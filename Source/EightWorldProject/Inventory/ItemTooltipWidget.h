#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Resources/ResourceDataTypes.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ItemTooltipWidget.generated.h"

UCLASS()
class EIGHTWORLDPROJECT_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	// 툴팁 업데이트
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateTooltip(const FItemData& InItemData);
	
	// 아이템 이름 텍스트
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemNameText;
	
	// 아이템 설명 텍스트
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ItemDescriptionText;
	
	// 아이템 아이콘
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* ItemIcon;
};
