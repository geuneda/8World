// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PWGameInstance.generated.h"


//세션(방) 정보 저장할 구조체
USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	FString playerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

	inline FString ToString() const
	{
		return FString::Printf(TEXT("[%d]%s : %s - %s, %dms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};


UCLASS()
class EIGHTWORLDPROJECT_API UPWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	//세션 만들기
	void CreateMySession(FString roomName, int32 playerCount);
public:
	IOnlineSessionPtr sessionInterface;

	//세션(방) 이름
	FString mySessionName = "Default Room";

	//호스트 사용자 이름
	FString myHostName = "Minsoo";

	//세션(방) 생성이 성공적으로 완료되면 호출되는 이벤트 콜백 함수
	UFUNCTION()
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);

public:
	//-------------방검색-----------------------
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	
	void FindOtherSessions();

	//방찾기 이벤트 콜백
	void OnFindSessionsComplete(bool bWasSuccessful);
};
