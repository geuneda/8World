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
	int32 pingSpeed = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 index = 0;

	inline FString ToString() const
	{
		return FString::Printf(TEXT("[%d]%s : %s - %s, %dms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};

//검색 완료시 호출되는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchComplete, const FSessionInfo&, sessionInfo);
//세션 검색 중일때 -> 비활성화 시키고 싶다. ->이때 사용될 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchStateSignature, bool, bIsSearching);
//미션 완료했을 때 호출되는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMissionComplete, int32, itemCount);

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

	//방찾기 완료 콜백을 등록할 델리게이트
	FSearchComplete OnSearchComplete;

	//방찾기 상태 콜백 델리게이트
	FSearchStateSignature OnSearchState;

public: //-----------세션(방) 입장 -------------------
	void JoinSelectedSession(int32 index);

	//방 입장 완료 체크 함수
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

public: //--------------미션-------------------
	FMissionComplete OnMissionComplete;
	
	//아이템 획득 개수
	UPROPERTY()
	int32 ItemCount = 0;
};
