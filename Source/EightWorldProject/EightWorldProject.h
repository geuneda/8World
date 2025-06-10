// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(EightWorld, Log, All);


#define NETMODE (GetWorld()->GetNetMode()==ENetMode::NM_Client?TEXT("Client") : \
GetWorld()->GetNetMode()==ENetMode::NM_Standalone?TEXT("Standalone") : TEXT("Server"))
//호출하는 함수와 줄번호 정보
#define APPINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINT_APPINFO() UE_LOG(EightWorld, Warning, TEXT("%s"), *APPINFO)
#define PRINTLOG(fmt,...) \
UE_LOG(EightWorld, Warning, TEXT("[%s]%s : %s"), NETMODE, *APPINFO,  *FString::Printf(fmt, ##__VA_ARGS__))

//Role
#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))