// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextWidget.h"

#include "Components/TextBlock.h"

void UDamageTextWidget::SetDamageText(int32 DamageAmount)
{
	if (txt_damage)
	{
		txt_damage->SetText(FText::AsNumber(DamageAmount));
	}

	if (FadeAndMove)
	{
		PlayAnimation(FadeAndMove);
	}
}
