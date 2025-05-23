// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerAttackComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

public:
	APlayerCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void MyJump(const FInputActionValue& Value);
			

protected:
	/** 달리기 상태 */
	bool bIsSprinting;

	/** 달리기 속도 */
	float SprintSpeed;

	/** 걷기 속도 */
	float WalkSpeed;

	/** 마나 소모 타이머 핸들 */
	FTimerHandle SprintManaTimerHandle;

	virtual void BeginPlay() override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	/** 플레이어 스탯 컴포넌트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStatComp")
	class UPlayerStatComp* PlayerStatComp;

	/** 플레이어 공격 컴포넌트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAttackComp")
	class UPlayerAttackComponent* PlayerAttackComp;

	/** 공격 입력 처리 */
	void Attack(const FInputActionValue& Value);

	/** 공격 입력 종료 처리 */
	void StopAttack(const FInputActionValue& Value);

	/** 달리기 입력 처리 */
	void Sprint(const FInputActionValue& Value);

	/** 달리기 입력 종료 처리 */
	void StopSprint(const FInputActionValue& Value);

	/** 달리기 상태 확인 */
	bool IsSprinting() const { return bIsSprinting; }

	/** 달리기 마나 소모 처리 */
	void ConsumeManaForSprint();

	/** TakeDamage 재정의 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


public: // ---------------------------UI_FUNC-------------------------------------

	void MainUIInit();

public: // ---------------------------UI-------------------------------------
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UMainUI* MainUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UMainUI> MainUIWidget;

};

