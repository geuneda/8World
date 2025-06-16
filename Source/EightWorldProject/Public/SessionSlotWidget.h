// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class EIGHTWORLDPROJECT_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	USessionSlotWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_roomName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_hostName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_playerCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_pingSpeed;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_join;

	int32 sessionNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ULoadingWidget> loadingWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ULoadingWidget* loadingWidget;
	
	void Set(const struct FSessionInfo& sessionInfo);

	//세션(방) 입장 함수
	UFUNCTION()
	void JoinSession();

public: //-------------사운드----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseHoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* mouseClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = Sound)
	USoundBase* enterIngameSound;
	
	UFUNCTION()
	void OnHoveredJoinButton();
	
};
