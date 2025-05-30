// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "BuildModeUI.h"
#include "Components/Image.h"
#include "Components/Widget.h"

void UMainUI::SetCrosshair(bool bIsVisible)
{
	Crosshair->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UMainUI::SetBuildModeUIVisible(bool bVisible)
{
	if (BuildModeUI)
	{
		BuildModeUI->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
