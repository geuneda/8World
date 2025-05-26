// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ResourceBase.h"
#include "Rock.generated.h"

/**
 * 돌 자원 클래스
 * 플레이어가 공격하여 자원을 채집할 수 있는 오브젝트
 */
UCLASS()
class EIGHTWORLDPROJECT_API ARock : public AResourceBase
{
	GENERATED_BODY()

public:
	ARock();

protected:
	// ResourceBase 가상 함수 구현
	virtual void HandleDeactivation() override;
	
	// 리소스 메시 로드 구현
	virtual void LoadResourceMesh() override;
};
