// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "PWGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "SessionSlotWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	
	//Create Room 바인딩
	btn_createRoom->OnClicked.AddDynamic(this, &ULobbyWidget::CreateRoom);
	//Slider PlayerCount 바인딩
	slider_playerCount->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnValueChanged);

	//widget switcher 이벤트
	btn_createSession->OnClicked.AddDynamic(this, &ULobbyWidget::SwitchCreatePanel);
	btn_findSession->OnClicked.AddDynamic(this, &ULobbyWidget::SwitchFindPanel);

	//뒤로가기
	btn_back->OnClicked.AddDynamic(this, &ULobbyWidget::BackToMain);
	btn_back_1->OnClicked.AddDynamic(this, &ULobbyWidget::BackToServerList);

	//세션 찾았을 때 호출될 콜백 함수 등록
	gi->OnSearchComplete.AddDynamic(this, &ULobbyWidget::AddSlotWidget);

	//방검색
	btn_find->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickedFindSession);

	//방 검색 중일때 호출될 콜백 함수 등록
	gi->OnSearchState.AddDynamic(this, &ULobbyWidget::OnFindStateEnable);

	//게임 시작 바인딩
	btn_gameStart->OnClicked.AddDynamic(this, &ULobbyWidget::ULobbyWidget::SwitchSelectServerPanel);
	//게임 종료 바인딩
	btn_gameQuit->OnClicked.AddDynamic(this, &ULobbyWidget::ULobbyWidget::SwitchGameQuit);

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

void ULobbyWidget::SwitchSelectServerPanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
	OnClickedFindSession();
}

void ULobbyWidget::SwitchGameQuit()
{
	//게임 종료
	if (auto pc = GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), pc, EQuitPreference::Quit, true);
	}
}

//방목록 검색 화면
void ULobbyWidget::SwitchCreatePanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void ULobbyWidget::SwitchFindPanel()
{
	//WidgetSwitcher->SetActiveWidgetIndex(3);
	OnClickedFindSession();
}

void ULobbyWidget::BackToMain()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULobbyWidget::BackToServerList()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyWidget::AddSlotWidget(const struct FSessionInfo& sessionInfo)
{
	//slot widget 만들어서 스크롤박스에 추가해주자
	auto slot = CreateWidget<USessionSlotWidget>(this, sessionInfoWidget);
	slot->Set(sessionInfo);

	//Scroll_roomList->AddChild(slot);
	vertical_roomList->AddChildToVerticalBox(slot);

}

void ULobbyWidget::OnClickedFindSession()
{
	//기존 슬롯이 있다면 모두 지운다.
	//Scroll_roomList->ClearChildren();
	vertical_roomList->ClearChildren();

	if (gi)
	{
		gi->FindOtherSessions();
	}
}

//방검색 중일때 검색 UI 표시 해주는 함수
void ULobbyWidget::OnFindStateEnable(bool bIsSearching)
{
	//검색 버튼은 비활성화
	btn_find->SetIsEnabled(!bIsSearching);

	//검색중... UI Visibility 속성 변경
	if (bIsSearching)
	{
		txt_findingMsg->SetVisibility((ESlateVisibility::Visible));
	}
	else
	{
		txt_findingMsg->SetVisibility((ESlateVisibility::Hidden));
	}

}
