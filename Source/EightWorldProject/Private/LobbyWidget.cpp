// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "LoadingWidget.h"
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

ULobbyWidget::ULobbyWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//Hover Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempHoverSound(TEXT("/Game/PalWorld/Sound/_ButtonHover_422836__gamedevc__g_ui_button_hover_1"));
	if (tempHoverSound.Succeeded())
	{
		mouseHoverSound = tempHoverSound.Object;
	}

	//Click Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempClickSound(TEXT("/Game/PalWorld/Sound/_ButtonClick_digital-click-357350"));
	if (tempClickSound.Succeeded())
	{
		mouseClickSound = tempClickSound.Object;
	}

	//EnterIngame Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempEnterSound(TEXT("/Game/PalWorld/Sound/_EnterIngame_665203__silverillusionist__level-upmission-complete-fantasy"));
	if (tempEnterSound.Succeeded())
	{
		enterIngameSound = tempEnterSound.Object;
	}

	//loading widget
	ConstructorHelpers::FClassFinder<ULoadingWidget> tempLoadingWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_Loading.WBP_Loading_C'"));
	if (tempLoadingWidget.Succeeded())
	{
		loadingWidgetClass = tempLoadingWidget.Class;
	}
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	
	//Create Room 바인딩
	btn_createRoom->OnClicked.AddDynamic(this, &ULobbyWidget::CreateRoom);
	btn_createRoom->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredCreateRoomButton);
	//Slider PlayerCount 바인딩
	slider_playerCount->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnValueChanged);

	//widget switcher 이벤트
	btn_createSession->OnClicked.AddDynamic(this, &ULobbyWidget::SwitchCreatePanel);
	btn_createSession->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredCreateSessionButton);
	btn_findSession->OnClicked.AddDynamic(this, &ULobbyWidget::SwitchFindPanel);

	//뒤로가기
	btn_back->OnClicked.AddDynamic(this, &ULobbyWidget::BackToMain);
	btn_back->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredBackButton);
	btn_back_1->OnClicked.AddDynamic(this, &ULobbyWidget::BackToServerList);
	btn_back_1->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredBack1Button);

	//세션 찾았을 때 호출될 콜백 함수 등록
	gi->OnSearchComplete.AddDynamic(this, &ULobbyWidget::AddSlotWidget);

	//방검색
	btn_find->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickedFindSession);
	btn_find->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredFindButton);

	//방 검색 중일때 호출될 콜백 함수 등록
	gi->OnSearchState.AddDynamic(this, &ULobbyWidget::OnFindStateEnable);

	//게임 시작 바인딩
	btn_gameStart->OnClicked.AddDynamic(this, &ULobbyWidget::ULobbyWidget::SwitchSelectServerPanel);
	//게임 종료 바인딩
	btn_gameQuit->OnClicked.AddDynamic(this, &ULobbyWidget::ULobbyWidget::SwitchGameQuit);

	//Mouse Hover
	btn_gameStart->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredStartButton);
	btn_gameQuit->OnHovered.AddDynamic(this, &ULobbyWidget::OnHoveredQuitButton);
	//Mouse UnHover
	btn_gameStart->OnUnhovered.AddDynamic(this, &ULobbyWidget::OnUnHoveredStartButton);
	btn_gameQuit->OnUnhovered.AddDynamic(this, &ULobbyWidget::OnUnHoveredQuitButton);
}

void ULobbyWidget::CreateRoom()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}

	if (loadingWidgetClass)
	{
		loadingWidget = CreateWidget<ULoadingWidget>(GetWorld(), loadingWidgetClass);
		if (loadingWidget && gi && edit_roomName->GetText().IsEmpty() == false)
		{
			loadingWidget->AddToViewport();
			loadingWidget->PlayAnimation(loadingWidget->FadeIn);
			
			const float TotalDelay = 2.0f;
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				if (enterIngameSound)
				{
					UGameplayStatics::PlaySound2D(this, enterIngameSound);
				}
				
				FTimerHandle ExitHandle;
				GetWorld()->GetTimerManager().SetTimer(ExitHandle, [&]()
				{
					//loadingWidget->PlayAnimation(loadingWidget->FadeOut);
					//값이 있을때 가져오기
					//if (gi && edit_roomName->GetText().IsEmpty() == false)
					//{
						FString roomName = edit_roomName->GetText().ToString();
						int32 playerCOunt = slider_playerCount->GetValue();
						gi->CreateMySession(roomName, playerCOunt);
					//}
				}, 1.5f, false);
				
			}, TotalDelay, false);
		}
	}
	
}

void ULobbyWidget::OnValueChanged(float value)
{
	txt_playerCount->SetText(FText::AsNumber(value));
}

void ULobbyWidget::SwitchSelectServerPanel()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	
	WidgetSwitcher->SetActiveWidgetIndex(1);
	OnClickedFindSession();
}

void ULobbyWidget::SwitchGameQuit()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	
	//게임 종료
	if (auto pc = GetWorld()->GetFirstPlayerController())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), pc, EQuitPreference::Quit, true);
	}
}

//방목록 검색 화면
void ULobbyWidget::SwitchCreatePanel()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void ULobbyWidget::SwitchFindPanel()
{
	//WidgetSwitcher->SetActiveWidgetIndex(3);
	OnClickedFindSession();
}

void ULobbyWidget::BackToMain()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULobbyWidget::BackToServerList()
{
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
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
	if (mouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseClickSound);
	}
	
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

void ULobbyWidget::OnHoveredStartButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
	FSlateFontInfo Startfont = txt_gameStart->GetFont();
	Startfont.Size = 35.f;
	txt_gameStart->SetFont(Startfont);
}

void ULobbyWidget::OnHoveredQuitButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
	FSlateFontInfo Quitfont = txt_gameQuit->GetFont();
	Quitfont.Size = 35.f;
	txt_gameQuit->SetFont(Quitfont);
}

void ULobbyWidget::OnUnHoveredStartButton()
{
	FSlateFontInfo Startfont = txt_gameStart->GetFont();
	Startfont.Size = 30.f;
	txt_gameStart->SetFont(Startfont);
}

void ULobbyWidget::OnUnHoveredQuitButton()
{
	FSlateFontInfo Quitfont = txt_gameQuit->GetFont();
	Quitfont.Size = 30.f;
	txt_gameQuit->SetFont(Quitfont);
}

void ULobbyWidget::OnHoveredFindButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void ULobbyWidget::OnHoveredBackButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void ULobbyWidget::OnHoveredCreateSessionButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void ULobbyWidget::OnHoveredCreateRoomButton()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}

void ULobbyWidget::OnHoveredBack1Button()
{
	if (mouseHoverSound)
	{
		UGameplayStatics::PlaySound2D(this, mouseHoverSound);
	}
}
