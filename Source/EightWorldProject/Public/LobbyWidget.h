// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
public: //-----------바인딩---------------
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_createRoom;
	UPROPERTY(meta = (BindWidget))
	class UEditableText* edit_roomName;
	UPROPERTY(meta = (BindWidget))
	class USlider* slider_playerCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_playerCount;

	//메인 -> 방 전환하도록 하는 위젯스위처, 방생성버튼, 방검색 버튼
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_gameStart;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_gameQuit;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_createSession;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_findSession;

	//뒤로 가기 버튼(main)
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_back;
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_back_1;

	//세션 슬롯 추가할 스크롤박스
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Scroll_roomList;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* vertical_roomList;
	
	UPROPERTY(EditDefaultsOnly, category = SlotWidget)
	TSubclassOf<class USessionSlotWidget> sessionInfoWidget;

	//방검색 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_find;

	//방검색 메세지
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_findingMsg;
	
	//GameInstance
	UPROPERTY()
	class UPWGameInstance* gi;
	
	UFUNCTION()
	void CreateRoom();

	//slider 값 변경하면 호출되는 이벤트 콜백 함수
	UFUNCTION()
	void OnValueChanged(float value);

	//화면 전환 함수
	UFUNCTION()
	void SwitchSelectServerPanel();
	UFUNCTION()
	void SwitchGameQuit();
	UFUNCTION()
	void SwitchCreatePanel();
	UFUNCTION()
	void SwitchFindPanel();

	//메인으로 돌아가는 함수
	UFUNCTION()
	void BackToMain();
	UFUNCTION()
	void BackToServerList();

	//슬롯 추가 함수
	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& sessionInfo);
	
	//방검색 버튼 클릭시 호출될 콜백
	UFUNCTION()
	void OnClickedFindSession();

	//방찾기 상태 이벤트 콜백
	UFUNCTION()
	void OnFindStateEnable(bool bIsSearching);
};
