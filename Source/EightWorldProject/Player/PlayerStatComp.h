// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EIGHTWORLDPROJECT_API UPlayerStatComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatComp();

public:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 플레이어
	class APlayerCharacter* Player;
	
	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HP)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HP)
	float MaxHP = 500.f;
	
	// 기력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP, ReplicatedUsing=OnRep_MP)
	float MP;
	UFUNCTION()
	void OnRep_MP();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP)
	float MaxMP = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MP)
	float RegenMPInterval = 1.0f;
	float RegenMPTimer;
	
	// 휴식중인지 여부
	UPROPERTY(Replicated)
	bool bIsRest;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// HP Getter/Setter
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetHP() const { return HP; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetHP(float NewHP);
	
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxHP() const { return MaxHP; }
	
	// MP Getter/Setter
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMP() const { return MP; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetMP(float NewMP);
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetMaxMP() const { return MaxMP; }
	
	// MP Regeneration
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RegenMP();
	
	// Rest State
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsResting() const { return bIsRest; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetRestState(bool bNewRestState) { bIsRest = bNewRestState; }


public:  //---------------네트워크--------------------
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
