// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemNotifyWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "EightWorldProject/EightWorldProject.h"

void UItemNotifyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemNotifyWidget::Setup(FText Name, UTexture2D* Icon)
{
	PRINTLOG(TEXT("Name : %s"),*Name.ToString())
	if (txt_itemName)
	{
		txt_itemName->SetText(Name);
	}

	if (itemicon && Icon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Icon);
		Brush.ImageSize = FVector2D(50.f, 50.f);
		Brush.DrawAs = ESlateBrushDrawType::Image;

		itemicon->SetBrushFromTexture(Icon, true);

	}
}
