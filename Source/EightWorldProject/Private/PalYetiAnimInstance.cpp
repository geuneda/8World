// Fill out your copyright notice in the Description page of Project Settings.


#include "PalYetiAnimInstance.h"

#include "PalYeti.h"

UPalYetiAnimInstance::UPalYetiAnimInstance()
{
}

void UPalYetiAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	yeti = Cast<APalYeti>(TryGetPawnOwner());
}

void UPalYetiAnimInstance::AnimNotify_DamageEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("[AnimNotify_DamageEnd] DamageEnd"));

	//데미지 애니메이션 취소
	if (this->bIsDamaged)
	{
		this->bIsDamaged = false;
		yeti->bIsDamaged = this->bIsDamaged;
	}
	
	//hp 30보다 더 클 때
	if (yeti->CurHP > 30)
	{
		//chase 상태 전환
		yeti->SetPalWildState(EPalWildState::Chase);
	}
	//hp 30 이하일 때
	else if (yeti->CurHP <= 30 && yeti->CurHP > 0)
	{
		yeti->SetPalWildState(EPalWildState::Escape);
	}
}
