// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildModeUI.h"
#include "Components/TextBlock.h"

void UBuildModeUI::SetBuildCost(int32 TreeHave, int32 TreeNeed, int32 RockHave, int32 RockNeed, bool bCanBuild)
{
    FSlateColor NormalColor = FSlateColor(FLinearColor::White);
    FSlateColor LackColor = FSlateColor(FLinearColor::Red);

    // 나무
    FString TreeText = FString::Printf(TEXT("%d / %d"), TreeHave, TreeNeed);
    if (TreeCostText)
    {
        TreeCostText->SetText(FText::FromString(TreeText));
        TreeCostText->SetColorAndOpacity(TreeHave >= TreeNeed ? NormalColor : LackColor);
    }

    // 돌
    FString RockText = FString::Printf(TEXT("%d / %d"), RockHave, RockNeed);
    if (RockCostText)
    {
        RockCostText->SetText(FText::FromString(RockText));
        RockCostText->SetColorAndOpacity(RockHave >= RockNeed ? NormalColor : LackColor);
    }
}
