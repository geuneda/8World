// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextActor.h"

#include "DamageTextWidget.h"
#include "Components/WidgetComponent.h"

// Sets default values
ADamageTextActor::ADamageTextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	RootComponent = WidgetComponent;
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); //화면 고정(정면)
	//WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(100, 50));

}

// Called when the game starts or when spawned
void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();

	// // 랜덤 Yaw 회전 (좌우 랜덤)
	// FRotator RandomRotation = GetActorRotation();
	// RandomRotation.Yaw += FMath::RandRange(-30.f, 30.f);
	// SetActorRotation(RandomRotation);
	
	//일정 시간 후 자동 제거
	SetLifeSpan(DestroyDelay);
}

// Called every frame
void ADamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageTextActor::SetDamageText(int32 DamageAmount)
{
	if (UUserWidget* Widget = WidgetComponent->GetUserWidgetObject())
	{
		UDamageTextWidget* DamageWidget = Cast<UDamageTextWidget>(Widget);
		 if (DamageWidget)
		 {
		 	DamageWidget->SetDamageText(DamageAmount);
		 }
	}
}
 
