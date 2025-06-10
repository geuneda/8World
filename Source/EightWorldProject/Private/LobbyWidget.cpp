// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "PWGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	
	//Create Room 바인딩
	btn_createRoom->OnClicked.AddDynamic(this, &ULobbyWidget::CreateRoom);
	//Slider PlayerCount 바인딩
	slider_playerCount->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnValueChanged);
}

void ULobbyWidget::CreateRoom()
{
	//값이 있을때 가져오기
	if (gi && edit_roomName->GetText().IsEmpty() == false)
	{
		FString roomName = edit_roomName->GetText().ToString();
		int32 playerCOunt = slider_playerCount->GetValue();
		gi->CreateMySession(roomName, playerCOunt);
	}
}

void ULobbyWidget::OnValueChanged(float value)
{
	txt_playerCount->SetText(FText::AsNumber(value));
}
