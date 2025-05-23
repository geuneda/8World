// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAttackComponent.h"
#include "PlayerStatComp.h"
#include "InputActionValue.h"
#include "PlayerAnimInstance.h"
#include "EightWorldProject/UI/MainUI.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// APlayerCharacter

APlayerCharacter::APlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 플레이어 스탯 컴포넌트 생성
	PlayerStatComp = CreateDefaultSubobject<UPlayerStatComp>(TEXT("PlayerStatComp"));
	
	// 플레이어 공격 컴포넌트 생성
	PlayerAttackComp = CreateDefaultSubobject<UPlayerAttackComponent>(TEXT("PlayerAttackComp"));

	ConstructorHelpers::FObjectFinder<UInputAction> attackInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_Attack.IA_Attack'"));
	if (attackInput.Succeeded())
	{
		AttackAction = attackInput.Object;
	}

	ConstructorHelpers::FClassFinder<UMainUI> mainUIWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_MainUI.WBP_MainUI_C'"));
	if (mainUIWidget.Succeeded())
	{
		MainUIWidget = mainUIWidget.Class;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		
		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAttack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// 공격 입력 처리
void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (PlayerAttackComp)
	{
		PlayerAttackComp->StartAttack();
	}
}

// 공격 입력 종료 처리
void APlayerCharacter::StopAttack(const FInputActionValue& Value)
{
	if (PlayerAttackComp)
	{
		PlayerAttackComp->StopAttack();
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::MyJump(const FInputActionValue& Value)
{
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;

	auto anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	anim->PlayJumpMontage();
}

void APlayerCharacter::BeginPlay()
{
	// 컴포넌트보다 먼저 UI 생성
	MainUIInit();
	
	Super::BeginPlay();
}

void APlayerCharacter::MainUIInit()
{
	auto pc = Cast<APlayerController>(GetController());
	if (!pc) return;

	if (!MainUIWidget) return;

	MainUI = Cast<UMainUI>(CreateWidget<UMainUI>(GetWorld(), MainUIWidget));
	MainUI->AddToViewport();
}

// TakeDamage 재정의
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 부모 클래스의 TakeDamage 호출
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 플레이어 스탯 컴포넌트가 있다면 체력 감소
	if (PlayerStatComp)
	{
		PlayerStatComp->SetHP(PlayerStatComp->GetHP()-ActualDamage);
		PlayerStatComp->SetHP(FMath::Clamp(PlayerStatComp->GetHP(), 0.0f, PlayerStatComp->GetMaxHP()));
		
		// 체력이 0 이하면 사망 처리 (추후 구현)
		if (PlayerStatComp->GetHP() <= 0.0f)
		{
			// 사망 처리 로직
		}
	}

	return ActualDamage;
}
