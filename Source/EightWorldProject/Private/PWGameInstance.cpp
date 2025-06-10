// Fill out your copyright notice in the Description page of Project Settings.


#include "PWGameInstance.h"
#include "OnlineSessionSettings.h"
#include "EightWorldProject/EightWorldProject.h"

//게임 시작시 호출
void UPWGameInstance::Init()
{
	Super::Init();

	if (auto subsys = IOnlineSubsystem::Get())
	{
		sessionInterface = subsys->GetSessionInterface(); //세션 인터페이스 가져오기

		//세션 이벤트 콜백 등록(델리게이트) - 세션이 정상적으로 만들어지면 추가가 된다.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPWGameInstance::OnCreateSessionComplete);
	}

	//세션 생성 (비동기 처리한다. 동기처리하면 값이 위에서 안들어오면 모든게 멈춘다. 연결이 완료되는 것까지 고려해서 지연시간을 두기 / 테스트용)
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
	{
		CreateMySession(mySessionName, 10);
	}), 2, false);
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
}
