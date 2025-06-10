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

	//GameInstance
	UPROPERTY()
	class UPWGameInstance* gi;
	
	UFUNCTION()
	void CreateRoom();

	//slider 값 변경하면 호출되는 이벤트 콜백 함수
	UFUNCTION()
	void OnValueChanged(float value);
};
