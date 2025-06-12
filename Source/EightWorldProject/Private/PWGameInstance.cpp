// Fill out your copyright notice in the Description page of Project Settings.


#include "PWGameInstance.h"
#include "OnlineSessionSettings.h"
#include "EightWorldProject/EightWorldProject.h"
#include "Online/OnlineSessionNames.h"

//게임 시작시 호출
void UPWGameInstance::Init()
{
	Super::Init();

	if (auto subsys = IOnlineSubsystem::Get())
	{
		sessionInterface = subsys->GetSessionInterface(); //세션 인터페이스 가져오기

		//세션 이벤트 콜백 등록(델리게이트) - 세션이 정상적으로 만들어지면 추가가 된다.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPWGameInstance::OnCreateSessionComplete);

		//세션 검색 이벤트 등록
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPWGameInstance::OnFindSessionsComplete);

		//세션 입장 이벤트 등록
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPWGameInstance::OnJoinSessionComplete);
	}

	// //세션 생성 (비동기 처리한다. 동기처리하면 값이 위에서 안들어오면 모든게 멈춘다. 연결이 완료되는 것까지 고려해서 지연시간을 두기 / 테스트용)
	// FTimerHandle handle;
	// GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
	// {
	// 	//CreateMySession(mySessionName, 10);
	// 	FindOtherSessions();
	// }), 2, false);

	
}

void UPWGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	//방을 만들려면 몇명, 비밀방인지, 우리 안에서 할지, 스팀에서 할지 등 설정할 데이터가 필요하다.
	//구조체
	FOnlineSessionSettings sessionSettings;

	//1. Dedicated server 접속여부(listn server 이용)
	sessionSettings.bIsDedicated = false;

	//2. 로컬(랜선)매칭 할지 혹은 steam 매칭을 사용할지 여부
	// DefaultEngine.ini 파일에 DefaultPlatformService=Steam 가져온다.
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = subsysName == "NULL"; //랜 매칭 - 스팀 사용 안할시

	//3. 매칭이 온라인을 통해 노출될지 여부(안보이게도 할 수 있음)
	sessionSettings.bShouldAdvertise = true;

	//4. 나의 온라인 상태(presence) 정보를 활용하게 해줄지 여부(상대방의 온라인 상태 등 확인 가능)
	sessionSettings.bUsesPresence = true;

	//5. 로비 사용여부
	sessionSettings.bUseLobbiesIfAvailable = true;

	//6. 게임진행중에 참여 허가할지 여부
	//보여지게 할지
	sessionSettings.bAllowJoinViaPresence = true;
	//게임참여 허가 할지
	sessionSettings.bAllowJoinInProgress = true;

	//7.세션에 참여할 수 있는 최대 참여자 수
	sessionSettings.NumPublicConnections = playerCount;

	//방이름, 호스트 이름, 몇명
	//8. 커스텀 룸네임 설정 	
	mySessionName = roomName;
	sessionSettings.Set(FName("ROOM_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//9. 호스트네임 설정
	sessionSettings.Set(FName("HOST_NAME"), myHostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//접속하기
	//NetID
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	PRINTLOG(TEXT("Create Session Start : %s"), *mySessionName);
	//방만들기
	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);
	
}

void UPWGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("SessionName : %s, bWasSuccessful : %d"), *sessionName.ToString(), bWasSuccessful);

	//방 만드는게 성공하면
	//->Server Travel (Game Server Open)
	if (bWasSuccessful)
	{
		//listen server로 입장
		GetWorld()->ServerTravel(TEXT("/Game/PalWorld/Maps/MyGameMap?listen?port=7777"));
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);
		PRINTLOG(TEXT("URL : %s"), *url);
	}
}

void UPWGameInstance::FindOtherSessions()
{
	//검색 시작 : 이때 화면 비활성화 시켜주기.
	OnSearchState.Broadcast(true);
	
	//찾을 조건들을 설정
	sessionSearch = MakeShareable(new FOnlineSessionSearch());  //MakeShareable 스마트포인터 참조포인터 사용.

	//1. 존재여부를 검색 가능하게 해 놓은 것만 찾기.
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	//2. Lan 사용여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");
	//3. 최대 검색 세션(방) 수
	sessionSearch->MaxSearchResults = 10;
	//4. 세션 검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UPWGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	//찾기 실패시 아무것도 하지 말기
	if (bWasSuccessful == false)
	{
		PRINTLOG(TEXT("Session search failed!!!"));
		return;
	}

	//로비 위젯 방검색 비활성화 종료
	OnSearchState.Broadcast(false);

	//세션 검색결과 배열
	auto results = sessionSearch->SearchResults;

	PRINTLOG(TEXT("Search Result Count : %d"), results.Num());

	for (int i = 0; i <results.Num(); i++)
	{
		auto sr = results[i];
		if (sr.IsValid() == false)
		{
			continue;
		}

		//세션 정보 저장할 구조체
		FSessionInfo sessionInfo;
		sessionInfo.index = i;
		
		
		// 방이름
		//FString roomName;
		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		//FString hostName;
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);
		//세션주인(머신)이름
		//FString userName = sr.Session.OwningUserName;
		//최대 입장가능한 플레이어 수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		//현재 입장한 플레이어 수 (최대 - 현재 입장가능한 수)
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);
		
		//핑정보
		sessionInfo.pingSpeed = sr.PingInMs; //밀리 sec 단위 / 신호를 보내서 다시 돌아올 때 시간 체크

		PRINTLOG(TEXT("%s"), *sessionInfo.ToString());
		
		//델리게이트로 위젯에 알려주기
		OnSearchComplete.Broadcast(sessionInfo);
	}
	
}

void UPWGameInstance::JoinSelectedSession(int32 index)
{
	//검색 결과 목록(배열)중 index 번째 녀석으로 방 입장하기.
	auto sr = sessionSearch->SearchResults[index];
	sr.Session.SessionSettings.Get(FName("ROOM_NAME"), mySessionName);
	sr.Session.SessionSettings.bUseLobbiesIfAvailable = true; // false로 잘못넘어오는 경우가 있는 경우 사용.
	sessionInterface->JoinSession(0, FName(mySessionName), sr);
}

void UPWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		//방으로 Client Travel
		auto pc = GetWorld()->GetFirstPlayerController();
		FString url;
		sessionInterface->GetResolvedConnectString(SessionName, url); // steam의 방정보를 받아와서 url로 넘겨줌
		PRINTLOG(TEXT("Join Session URL : %s"), *url);
		if (url.IsEmpty() == false)
		{
			pc->ClientTravel(url, TRAVEL_Absolute); //맵을 이동할 때 모든 설정을 리셋하고 들어간다.
		}
	}
	else
	{
		PRINTLOG(TEXT("JoinSessionComplete Failed : %d"), Result);
	}


}
