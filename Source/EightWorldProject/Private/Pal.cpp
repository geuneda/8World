// Fill out your copyright notice in the Description page of Project Settings.


#include "Pal.h"

// Sets default values
APal::APal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DataTable 추가하기
	ConstructorHelpers::FObjectFinder<UDataTable> tempDataTable(TEXT("'/Game/PalWorld/DataTables/DT_PalInfoData.DT_PalInfoData'"));
	if (tempDataTable.Succeeded())
	{
		PalDataTable = tempDataTable.Object;
	}
}

// Called when the game starts or when spawned
void APal::BeginPlay()
{
	Super::BeginPlay();

	//DataTable 로딩
	if (PalDataTable)
	{
		FPalInfoData* InfoData = PalDataTable->FindRow<FPalInfoData>(PalDataRowName, TEXT("Alpaca"));
		if (InfoData)
		{
			PalInfo = *InfoData;
		}
	}
	
	
}

// Called every frame
void APal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APal::UpdateFSM(float DeltaTime)
{
}

