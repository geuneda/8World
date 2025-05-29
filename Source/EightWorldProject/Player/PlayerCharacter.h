// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputActionValue.h"
#include "../Inventory/InventoryComponent.h"
#include "../Inventory/InventoryWidget.h"
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

	/** Inventory Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventoryAction;

	/** Build Mode Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BuildModeAction;

	/** 마우스 휠 Up/Down */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseWheelUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseWheelDownAction;

public:
	APlayerCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void MyJump(const FInputActionValue& Value);
			

protected:

	/** 빌드 상태 */
	bool bIsBuilding = false;
	
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

	/** 플레이어 빌드 시스템 컴포넌트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerBuildComp")
	class UBuildComponent* PlayerBuildComp;

	/** 아이템 습득 처리 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(class AResourceItem* Item);

	/** 공격 입력 처리 */
	void Attack(const FInputActionValue& Value);

	/** 공격 입력 종료 처리 */
	void StopAttack(const FInputActionValue& Value);

	/** 달리기 입력 처리 */
	void Sprint(const FInputActionValue& Value);

	/** 달리기 입력 종료 처리 */
	void StopSprint(const FInputActionValue& Value);

	/** 건축 모드 처리 */
	void BuildMode(const FInputActionValue& Value);

	/** 마우스 휠 업/다운 처리 */
	void MouseWheelDown(const FInputActionValue& Value);
	void MouseWheelUp(const FInputActionValue& Value);

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

	// 인벤토리 UI 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	// 인벤토리 UI 위젯 인스턴스
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	class UInventoryWidget* InventoryWidget;

	// 인벤토리 UI 표시/숨김 처리
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleInventory();
	
	// 인벤토리가 열려있는지 확인
	bool IsInventoryOpen() const { return bIsInventoryOpen; }
	
	// 인벤토리 상태 변수
	bool bIsInventoryOpen = false;

	// 인벤토리 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	class UInventoryComponent* InventoryComponent;
};

