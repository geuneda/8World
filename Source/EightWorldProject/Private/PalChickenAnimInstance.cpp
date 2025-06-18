// Fill out your copyright notice in the Description page of Project Settings.


#include "PalChickenAnimInstance.h"

#include "PalChicken.h"
#include "Components/AudioComponent.h"

UPalChickenAnimInstance::UPalChickenAnimInstance()
{
}

void UPalChickenAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Chicken = Cast<APalChicken>(TryGetPawnOwner());
}

void UPalChickenAnimInstance::AnimNotify_DamageAnimEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("[AnimNotify_DamageAnimEnd] DamageEnd"));
	
	
	//데미지 애니메이션 취소
	if (this->bIsDamaged)
	{
		this->bIsDamaged = false;
		Chicken->bIsDamaged = this->bIsDamaged;
	}

	//hp 0보다 더 클 때
	if (Chicken->CurHP > 0)
	{
		//Escape 상태 전환
		Chicken->SetPalWildState(EPalWildState::Escape);
	}
	//hp 0 이하일 때
	else if (Chicken->CurHP <= 0)
	{
		Chicken->SetPalWildState(EPalWildState::Die);
	}
}
