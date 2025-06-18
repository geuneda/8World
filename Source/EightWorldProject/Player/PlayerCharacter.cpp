// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerCharacter.h"

#include "DamageTextActor.h"
#include "DeathWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ESCWidget.h"
#include "GoalWidget.h"
#include "PlayerAttackComponent.h"
#include "PlayerStatComp.h"
#include "InputActionValue.h"
#include "ItemNotifyWidget.h"
#include "MissionCompleteWidget.h"
#include "PlayerAnimInstance.h"
#include "PWGameInstance.h"
#include "PWGameState.h"
#include "PWPlayerController.h"
#include "EightWorldProject/Resources/ResourceItem.h"
#include "EightWorldProject/UI/MainUI.h"
#include "Engine/LocalPlayer.h"
#include "../Inventory/InventoryComponent.h"
#include "../Inventory/InventoryWidget.h"
#include "Components/AudioComponent.h"
#include "Components/CheckBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "EightWorldProject/EightWorldProject.h"
#include "EightWorldProject/BuildSystem/BuildComponent.h"
#include "EightWorldProject/Resources/RockItem.h"
#include "EightWorldProject/Resources/TreeItem.h"
#include "EightWorldProject/UI/MPUI.h"
#include "Kismet/GameplayStatics.h"

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
	WalkSpeed = 300.f;
	SprintSpeed = 800.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	// 달리기 상태 초기화
	bIsSprinting = false;
	
	// 줌 상태 초기화
	bIsZooming = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultArmLength; // The camera follows at this distance behind the character
	CameraBoom->SocketOffset = FVector(0.0f, 60.0f, 60.0f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// 플레이어 스탯 컴포넌트 생성
	PlayerStatComp = CreateDefaultSubobject<UPlayerStatComp>(TEXT("PlayerStatComp"));
	
	// 플레이어 공격 컴포넌트 생성
	PlayerAttackComp = CreateDefaultSubobject<UPlayerAttackComponent>(TEXT("PlayerAttackComp"));
	
	// 인벤토리 컴포넌트 생성
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 빌드 시스템 컴포넌트 생성
	PlayerBuildComp = CreateDefaultSubobject<UBuildComponent>(TEXT("PlayerBuildComp"));

	// 팰스피어 컴포넌트 생성
	PalSphereComp = CreateDefaultSubobject<UPalSphereComponent>(TEXT("PalSphereComp"));

	ConstructorHelpers::FObjectFinder<UInputAction> attackInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_Attack.IA_Attack'"));
	if (attackInput.Succeeded())
	{
		AttackAction = attackInput.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> sprintInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_Sprint.IA_Sprint'"));
	if (sprintInput.Succeeded())
	{
		SprintAction = sprintInput.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> inventoryInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_Inventory.IA_Inventory'"));
	if (inventoryInput.Succeeded())
	{
		InventoryAction = inventoryInput.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> buildModeInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_BuildMode.IA_BuildMode'"));
	if (buildModeInput.Succeeded())
	{
		BuildModeAction = buildModeInput.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> mouseWheelDownInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_WheelDown.IA_WheelDown'"));
	if (mouseWheelDownInput.Succeeded())
	{
		MouseWheelDownAction = mouseWheelDownInput.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> palSphereInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_PalSphere.IA_PalSphere'"));
	if (palSphereInput.Succeeded())
	{
		PalSphereAction = palSphereInput.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> zoomInInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_ZoomIn.IA_ZoomIn'"));
	if (zoomInInput.Succeeded())
	{
		ZoomInAction = zoomInInput.Object;
	}

	ConstructorHelpers::FClassFinder<UMainUI> mainUIWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_MainUI.WBP_MainUI_C'"));
	if (mainUIWidget.Succeeded())
	{
		MainUIWidget = mainUIWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UGoalWidget> goalUIWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_GoalUI.WBP_GoalUI_C'"));
	if (goalUIWidget.Succeeded())
	{
		GoalUIWiget = goalUIWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UMissionCompleteWidget> missionWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_MissionComplete.WBP_MissionComplete_C'"));
	if (missionWidget.Succeeded())
	{
		missionCompleteUIWidget = missionWidget.Class;
	}

	//FootStep Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempFootStepSound(TEXT("/Game/PalWorld/Sound/SC_FootStep"));
	if (tempFootStepSound.Succeeded())
	{
		footStepSound = tempFootStepSound.Object;
	}
	
	SetReplicates(true);
	SetReplicateMovement(true);

	
	//Footstep sound setting
	footStepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	footStepAudioComponent->SetupAttachment(RootComponent);
	footStepAudioComponent->SetSound(footStepSound);
	footStepAudioComponent->bAutoActivate = false;


	//Hit Air Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitAirSound(TEXT("/Game/PalWorld/Sound/SC_AirPunch"));
	if (tempHitAirSound.Succeeded())
	{
		HitAirSound = tempHitAirSound.Object;
	}
	//Hit Resource Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitResourceSound(TEXT("/Game/PalWorld/Sound/SC_ResourcePunch"));
	if (tempHitResourceSound.Succeeded())
	{
		HitResourceSound = tempHitResourceSound.Object;
	}
	//Hit Pal Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitPalSound(TEXT("/Game/PalWorld/Sound/SC_PalPunch"));
	if (tempHitPalSound.Succeeded())
	{
		HitPalSound = tempHitPalSound.Object;
	}

	//Item Get Sound
	ConstructorHelpers::FObjectFinder<USoundBase> tempItemGetSound(TEXT("/Game/PalWorld/Sound/SC_ItemGet"));
	if (tempItemGetSound.Succeeded())
	{
		ItemGetSound = tempItemGetSound.Object;
	}

	//Shake Class
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempPalShake(TEXT("/Script/Engine.Blueprint'/Game/PalWorld/Blueprints/BP_PalHitCameraShake.BP_PalHitCameraShake_C'"));
	if (tempPalShake.Succeeded())
	{
		PalHitShakeClass = tempPalShake.Class;
	}
	ConstructorHelpers::FClassFinder<UCameraShakeBase> tempResourceShake(TEXT("/Script/Engine.Blueprint'/Game/PalWorld/Blueprints/BP_ResourceHitCameraShake.BP_ResourceHitCameraShake_C'"));
	if (tempResourceShake.Succeeded())
	{
		ResourceHitShakeClass = tempResourceShake.Class;
	}

	ConstructorHelpers::FClassFinder<UDeathWidget> deathWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_Death.WBP_Death_C'"));
	if (deathWidget.Succeeded())
	{
		DeathWidgetClass = deathWidget.Class;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> escInput(TEXT("/Script/EnhancedInput.InputAction'/Game/PalWorld/Input/Actions/IA_ESC.IA_ESC'"));
	if (escInput.Succeeded())
	{
		ESCAction = escInput.Object;
	}

	ConstructorHelpers::FClassFinder<UESCWidget> escWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_ESC.WBP_ESC_C'"));
	if (escWidget.Succeeded())
	{
		ESCWidgetClass = escWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UESCWidget> itemNotifyWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/PalWorld/UI/WBP_ItemNotify.WBP_ItemNotify_C'"));
	if (itemNotifyWidget.Succeeded())
	{
		ItemNotifyWidgetClass = itemNotifyWidget.Class;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> tempplayerSound(TEXT("/Game/PalWorld/Sound/playerDeath"));
	if (tempplayerSound.Succeeded())
	{
		playerDeathSound = tempplayerSound.Object;
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
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MyJump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		
		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAttack);
		
		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		
		EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventory);

		EnhancedInputComponent->BindAction(BuildModeAction, ETriggerEvent::Started, this, &APlayerCharacter::BuildMode);

		EnhancedInputComponent->BindAction(MouseWheelDownAction, ETriggerEvent::Started, this, &APlayerCharacter::MouseWheelDown);
		EnhancedInputComponent->BindAction(MouseWheelUpAction, ETriggerEvent::Started, this, &APlayerCharacter::MouseWheelUp);
		
		// 팰스피어 액션 바인딩
		EnhancedInputComponent->BindAction(PalSphereAction, ETriggerEvent::Started, this, &APlayerCharacter::ThrowPalSphere);
		
		// 줌 인/아웃 입력 바인딩
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Started, this, &APlayerCharacter::ZoomIn);
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Completed, this, &APlayerCharacter::ZoomOut);

		//esc to title
		EnhancedInputComponent->BindAction(ESCAction, ETriggerEvent::Started, this, &APlayerCharacter::ESCToTitle);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// 공격 입력 처리
void APlayerCharacter::Attack(const FInputActionValue& Value)
{
	// if (PlayerBuildComp->bIsBuildMode && PlayerBuildComp->bCanBuild  && !bIsBuilding)
	// {
	// 	bIsBuilding = true;
	// 	
	// 	PlayerBuildComp->SpawnBuild();
	//
	// 	FTimerHandle BuildTimerHandle;
	// 	GetWorldTimerManager().SetTimer(BuildTimerHandle, [&]()
	// 	{
	// 		bIsBuilding = false;
	// 	}, 1.0f, false);
	// 	
	// 	return;
	// }
	//
	//
	// if (PlayerAttackComp && !IsInventoryOpen())
	// {
	// 	PlayerAttackComp->StartAttack();
	// }
	
	ServerRPC_Attack();
}

void APlayerCharacter::ServerRPC_Attack_Implementation()
{
	if (PlayerBuildComp->bIsBuildMode && PlayerBuildComp->bCanBuild  && !bIsBuilding)
	{
		bIsBuilding = true;
		
		PlayerBuildComp->SpawnBuild();

		FTimerHandle BuildTimerHandle;
		GetWorldTimerManager().SetTimer(BuildTimerHandle, [&]()
		{
			bIsBuilding = false;
		}, 1.0f, false);
		
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("공격!"));
	if (PlayerAttackComp && !IsInventoryOpen())
	{
		PlayerAttackComp->StartAttack();
	}
}

void APlayerCharacter::MultiRPC_Attack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[MultiRPC_Attack] AttackMontage Start"));
	if (UPlayerAnimInstance* anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (anim->AttackMontage)
		{
			anim->Montage_Play(anim->AttackMontage, 1.0f);
		}
	}
}

void APlayerCharacter::Multicast_PlayCameraShake_Implementation(bool bPalHit, bool bResourceHit)
{
	APWPlayerController* PC = Cast<APWPlayerController>(GetController());

	// 로컬 플레이어만 실행 (본인 클라만)
	if (PC && PC->IsLocalController())
	{
		if (bPalHit && PalHitShakeClass)
		{
			PC->ClientStartCameraShake(PalHitShakeClass);
		}
		else if (bResourceHit && ResourceHitShakeClass)
		{
			PC->ClientStartCameraShake(ResourceHitShakeClass);
		}
	}
}

// 공격 입력 종료 처리
void APlayerCharacter::StopAttack(const FInputActionValue& Value)
{
	// if (PlayerAttackComp)
	// {
	// 	PlayerAttackComp->StopAttack();
	// }

	ServerRPC_StopAttack();
}

void APlayerCharacter::ServerRPC_StopAttack_Implementation()
{
	if (PlayerAttackComp)
	{
		PlayerAttackComp->StopAttack();
	}
}

void APlayerCharacter::MultiRPC_StopAttack_Implementation()
{
	if (UPlayerAnimInstance* anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (anim->AttackMontage)
		{
			anim->Montage_Stop(0.25f, anim->AttackMontage);
			UE_LOG(LogTemp, Warning, TEXT("[MultiRPC_StopAttack] AttackMontage Stop"));
		}
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
	if (IsInventoryOpen()) return;
	
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

	// auto anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	//
	// anim->PlayJumpMontage();
	ServerRPC_Jump();
}

void APlayerCharacter::ServerRPC_Jump_Implementation()
{
	//bPressedJump = true;
	//JumpKeyHoldTime = 0.0f;
	
	MultiRPC_Jump();

}

void APlayerCharacter::MultiRPC_Jump_Implementation()
{
	auto anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	anim->PlayJumpMontage();
}

void APlayerCharacter::MouseWheelDown(const FInputActionValue& Value)
{
	if (PlayerBuildComp->bIsBuildMode)
	{
		PlayerBuildComp->BuildID = FMath::Clamp(PlayerBuildComp->BuildID+1, 0, PlayerBuildComp->BuildData.Num() - 1);

		PlayerBuildComp->ChangeMesh();
	}
}

void APlayerCharacter::MouseWheelUp(const FInputActionValue& Value)
{
	if (PlayerBuildComp->bIsBuildMode)
	{
		PlayerBuildComp->BuildID = FMath::Clamp(PlayerBuildComp->BuildID-1, 0, PlayerBuildComp->BuildData.Num() - 1);

		PlayerBuildComp->ChangeMesh();
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		MainUIInit();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 컴포넌트보다 먼저 UI 생성
	if (IsLocallyControlled() && HasAuthority() == false )
	{
		MainUIInit();
		
	}
	
	// 인벤토리 UI 생성
	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->InitializeInventory(InventoryComponent);
		}
	}

	// Build 컴포넌트 카메라 변수 주입
	if (PlayerBuildComp)
	{
		PlayerBuildComp->CameraComp = FollowCamera;
	}

	GI = Cast<UPWGameInstance>(GetWorld()->GetGameInstance());
	
	

	GS = Cast<APWGameState>(GetWorld()->GetGameState());
	
}

void APlayerCharacter::MainUIInit()
{
	auto pc = Cast<APlayerController>(GetController());
	if (!pc) return;

	if (!MainUIWidget) return;

	MainUI = Cast<UMainUI>(CreateWidget<UMainUI>(GetWorld(), MainUIWidget));
	MainUI->AddToViewport();
	
	// 초기에 크로스헤어 숨기기
	MainUI->SetCrosshair(false);

	if (IsLocallyControlled())
	{
		GoalUI = Cast<UGoalWidget>(CreateWidget(GetWorld(), GoalUIWiget));
		GoalUI->AddToViewport();
		
		if (GI && GI->GetItemCount >= 10)
		{
			MultiRPC_ItemCount_Implementation(true);
		}

		missionCompleteUI = Cast<UMissionCompleteWidget>(CreateWidget(GetWorld(), missionCompleteUIWidget));
	}
}

// 달리기 입력 처리
void APlayerCharacter::Sprint(const FInputActionValue& Value)
{

	// // 마나가 충분한지 확인
	// if (PlayerStatComp && PlayerStatComp->GetMP() > 0)
	// {
	// 	bIsSprinting = true;
	// 	
	// 	// 달리기 속도로 변경
	// 	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	// 	
	// 	// 휴식 상태 해제
	// 	PlayerStatComp->SetRestState(false);
	// 	
	// 	// 마나 소모 타이머 시작
	// 	GetWorldTimerManager().SetTimer(SprintManaTimerHandle, this, &APlayerCharacter::ConsumeManaForSprint, 1.0f, true, 0.0f);
	// }

	ServerRPC_Sprint();
}

void APlayerCharacter::ServerRPC_Sprint_Implementation()
{
	// 마나가 충분한지 확인
	if (PlayerStatComp && PlayerStatComp->GetMP() > 0)
	{
		bIsSprinting = true;

		PitchValue = 1.5f;
		footStepAudioComponent->Stop();
		footStepAudioComponent->PitchMultiplier = PitchValue;
		
		// 달리기 속도로 변경
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		
		// 휴식 상태 해제
		PlayerStatComp->SetRestState(false);

		if (!GetWorldTimerManager().IsTimerActive(SprintManaTimerHandle))
		{
			// 마나 소모 타이머 시작
			GetWorldTimerManager().SetTimer(SprintManaTimerHandle, this, &APlayerCharacter::ConsumeManaForSprint, 1.0f, true, 0.0f);
			
		}
	}
}


// 달리기 입력 종료 처리
void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("StopSprint / bIsSprinting : %d"), bIsSprinting);
	if (bIsSprinting)
	{
		ServerRPC_SprintStop();
		
	}
}

void APlayerCharacter::ServerRPC_SprintStop_Implementation()
{
	bIsSprinting = false;

	PitchValue = 1.f;
	footStepAudioComponent->Stop();
	footStepAudioComponent->PitchMultiplier = PitchValue;
	
	// 걷기 속도로 복귀
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		
	// 휴식 상태로 변경
	if (PlayerStatComp)
	{
		PlayerStatComp->SetRestState(true);
	}
		
	// 마나 소모 타이머 중지
	GetWorldTimerManager().ClearTimer(SprintManaTimerHandle);
}

void APlayerCharacter::OnRep_PitchMultiplier()
{
	footStepAudioComponent->Stop();
	footStepAudioComponent->PitchMultiplier = PitchValue;
}

void APlayerCharacter::OnRep_CheckSprint()
{
	if (bIsSprinting)
	{
		//bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		//bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::OnRep_IsSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::OnRep_IsWalking()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::BuildMode(const FInputActionValue& Value)
{
	if (PlayerBuildComp)
	{
		PlayerBuildComp->LaunchBuildMode();
	}
}

// 달리기 마나 소모 처리
void APlayerCharacter::ConsumeManaForSprint()
{
	if (PlayerStatComp && bIsSprinting)
	{
		// 마나 감소
		float CurrentMP = PlayerStatComp->GetMP();
		PlayerStatComp->SetMP(CurrentMP - 5.0f);
		
		// 마나가 0이 되면 달리기 중지
		if (PlayerStatComp->GetMP() <= 0)
		{
			StopSprint(FInputActionValue());
		}
	}
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

		//PRINTLOG(TEXT("Player : %s, PlayerStatComp GetHP : %f"), *this->GetName(), PlayerStatComp->GetHP());
		
		//MainUI->hp = PlayerStatComp->GetHP() / PlayerStatComp->GetMaxHP();
		//OnRep_MyTakeDamage();
		ClientRPC_MyTakeDamage(PlayerStatComp->GetHP() / PlayerStatComp->GetMaxHP());
		
		// 체력이 0 이하면 사망 처리 (추후 구현)
		if (PlayerStatComp->GetHP() <= 0.0f)
		{
			// 사망 처리 로직
			PRINTLOG(TEXT("player : %s Died"), *Cast<APlayerCharacter>(PlayerStatComp->GetOwner())->GetName());

			//플레이어 죽음 알림 델리게이트
			OnPlayerDead.Broadcast();
			bIsDead = true;
			
			//RagDoll
			//물리 시뮬레이션 활성화
			GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			GetMesh()->SetSimulatePhysics(true);

			//캡슐 콜라이더 비활성화
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);\

			MultiRPC_PlayerDie();

			//죽음 UI
			FTimerHandle deathTimerHandle;
			GetWorldTimerManager().SetTimer(deathTimerHandle, this, &APlayerCharacter::OpenDeathWidget, 0.1f, false);
		}
	}

	return ActualDamage;
}

void APlayerCharacter::OnRep_MyTakeDamage()
{
	// if (MainUI && IsLocallyControlled())
	// {
	// 	MainUI->hp = PlayerStatComp->GetHP() / PlayerStatComp->GetMaxHP();
	// }
}

void APlayerCharacter::ClientRPC_MyTakeDamage_Implementation(float damgePercent)
{
	if (MainUI)
	{
		MainUI->hp = damgePercent;
	}
}

void APlayerCharacter::MultiRPC_PlayerDie_Implementation()
{
	//RagDoll
	//물리 시뮬레이션 활성화
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	//캡슐 콜라이더 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APlayerCharacter::ClientRPC_DeathWidget_Implementation()
{
	PRINTLOG(TEXT("ClientRPC_DeathWidget"));
	auto pc = Cast<APlayerController>(GetController());
	if (!pc) return;

	if (playerDeathSound)
	{
		UGameplayStatics::PlaySound2D(this, playerDeathSound);
	}

	FTimerHandle deathTimerHandle;
	auto DeathEvent = [&, pc]()
	{
		if (DeathWidgetClass)
		{
			DeathWidget = Cast<UDeathWidget>(CreateWidget<UDeathWidget>(GetWorld(), DeathWidgetClass));
			if (DeathWidget)
			{
				DeathWidget->AddToViewport();
				GoalUI->SetVisibility(ESlateVisibility::Hidden);
				MainUI->MPUI->SetVisibility(ESlateVisibility::Hidden);
				MainUI->HealthBar->SetVisibility(ESlateVisibility::Hidden);
				MainUI->txt_hp->SetVisibility(ESlateVisibility::Hidden);
	
				if (DeathWidget->gameOverSound)
				{
					UGameplayStatics::PlaySound2D(this, DeathWidget->gameOverSound);
				}
				DeathWidget->PlayAnimation(DeathWidget->Opacity);
			
				// GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, [this]()
				// {
				// 	DeathWidget->PlayAnimation(DeathWidget->Repeat, 0.f, -1, EUMGSequencePlayMode::Forward);
				// }, 1.6f, false); 
			
			
				//컨트롤러 제거
				//DetachFromControllerPendingDestroy();
			}

			//입력 모드를 UI Only로 변경
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(DeathWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 혹은 LockAlways
			pc->SetInputMode(InputMode);

			//마우스 커서 표시
			pc->bShowMouseCursor = true;
		}
	};
	
	GetWorldTimerManager().SetTimer(deathTimerHandle, FTimerDelegate::CreateLambda(DeathEvent), 1.0f, false);
}

void APlayerCharacter::OpenDeathWidget()
{
	ClientRPC_DeathWidget();
}

void APlayerCharacter::PickupItem(AResourceItem* Item)
{
	if (!Item || !InventoryComponent)
	{
		return;
	}
	//줍기 화면 UI
	ClientRPC_ShowItemNotification(Item);
	
	// 아이템을 인벤토리에 추가
	bool bAdded = InventoryComponent->AddItem(Item->ResourceID, Item->Quantity);
	
	// 인벤토리에 추가 성공하면 아이템 제거
	if (bAdded)
	{
		PRINTLOG(TEXT("[PickupItem] 아이템 인벤토리 추가 성공"));

		//줍기 사운드 실행
		MultiRPC_ItemGetSound(Item->GetActorLocation());
		
		Item->Destroy();
	}
	else
	{
		// 인벤토리에 추가 실패시 에러 로그 출력
		UE_LOG(LogTemplateCharacter, Error, TEXT("인벤토리에 추가 실패"));
	}

	if (IsLocallyControlled())
	{
	// 	MultiRPC_ItemCount(true);
	// 	FString NetMode = GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server");
	// 	UE_LOG(LogTemp, Warning, TEXT("[%s] [PickupItem]"), *NetMode);
	// }
	// else
	// {
		FString NetMode = GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server");
		
		if (GI)
		{
			if (GetNetMode() == NM_Client)
			{
				ServerRPC_Count();
			}
			else
			{
				GI->GetItemCount++;
				if (GS)
				{
					GS->SharedItemCount = GI->GetItemCount;
					GS->SetSharedItemCount(GS->SharedItemCount);
				}
				PRINTLOG(TEXT("[PickupItem] ItemCount: %d"), GI->GetItemCount);
			}
			
			if (GI->GetItemCount >= 20)
			{
				ServerRPC_ItemCount(true);
				
				UE_LOG(LogTemp, Warning, TEXT("[%s] [PickupItem]"), *NetMode);
			}
		}
		
	}
	
}

void APlayerCharacter::ClientRPC_ShowItemNotification_Implementation(AResourceItem* Item)
{
	auto pc = Cast<APWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!pc) return;

	if (auto tree = Cast<ATreeItem>(Item))
	{
		txtName = FText::FromString(TEXT("통나무 + 1"));
		ShowItemPickupNotification(txtName, TreeIcon);
	}
	else if (auto rock = Cast<ARockItem>(Item))
	{
		txtName = FText::FromString(TEXT("돌맹이 + 1"));
		ShowItemPickupNotification(txtName, StoneIcon);
	}
}

void APlayerCharacter::ServerRPC_Count_Implementation()
{
	GI->GetItemCount++;
	if (GS)
	{
		GS->SharedItemCount = GI->GetItemCount;
		GS->SetSharedItemCount(GS->SharedItemCount);
	}
	PRINTLOG(TEXT("[PickupItem] ItemCount: %d"), GI->GetItemCount);
	FString NetMode = GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server");
	UE_LOG(LogTemp, Warning, TEXT("[%s] [PickupItem]"), *NetMode);
}

void APlayerCharacter::MultiRPC_ItemGetSound_Implementation(FVector location)
{
	//아이템 줍기 사운드
	if (ItemGetSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
		this,
		ItemGetSound,
		location,
		FRotator::ZeroRotator,
		1.0f,
		1.0f,
		0.0f,
		HitSoundAttenuation,
		nullptr,
		true  // bAutoDestroy
	);
	}
}

void APlayerCharacter::ThrowPalSphere(const FInputActionValue& Value)
{
	// // 인벤토리가 열려있거나 빌드 모드일 경우 실행하지 않음
	// if (IsInventoryOpen() || PlayerBuildComp->bIsBuildMode)
	// {
	// 	return;
	// }
	//
	// // 애니메이션 인스턴스 가져오기
	// UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	// if (AnimInstance)
	// {
	// 	// 팰스피어 애니메이션 재생
	// 	AnimInstance->PlayPalSphereMontage();
	// 	
	// 	// 휴식 상태 해제
	// 	if (PlayerStatComp)
	// 	{
	// 		PlayerStatComp->SetRestState(false);
	// 	}
	// }

	ServerRPC_Sphere();
}

void APlayerCharacter::ServerRPC_Sphere_Implementation()
{
	// 인벤토리가 열려있거나 빌드 모드일 경우 실행하지 않음
	if (IsInventoryOpen() || PlayerBuildComp->bIsBuildMode)
	{
		return;
	}

	// 애니메이션 인스턴스 가져오기
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		// 팰스피어 애니메이션 재생
		AnimInstance->PlayPalSphereMontage();
		
		// 휴식 상태 해제
		if (PlayerStatComp)
		{
			PlayerStatComp->SetRestState(false);
		}
	}
}

void APlayerCharacter::MultiRPC_Sphere_Implementation()
{
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance->PalSphereMontage)
	{
		AnimInstance->Montage_Play(AnimInstance->PalSphereMontage, 1.0f);
	}
}

void APlayerCharacter::ToggleInventory()
{
	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			InventoryWidget->RemoveFromParent();
			bIsInventoryOpen = false;
			
			// 마우스 커서 해제 및 게임 입력 모드로 변경
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				PC->SetInputMode(FInputModeGameOnly());
				PC->bShowMouseCursor = false;
			}
		}
		else
		{
			InventoryWidget->AddToViewport();
			
			// 마우스 커서 표시 및 UI 입력 모드로 변경
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputMode.SetHideCursorDuringCapture(false);
				
				if (InventoryWidget)
				{
					InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
				}

				bIsInventoryOpen = true;
				
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
		}
	}
}

void APlayerCharacter::ZoomIn(const FInputActionValue& Value)
{
	// 인벤토리가 열려있거나 빌드 모드일 경우 실행하지 않음
	if (IsInventoryOpen() || (PlayerBuildComp && PlayerBuildComp->bIsBuildMode))
	{
		return;
	}

	bIsZooming = true;

	// 크로스헤어 표시
	if (MainUI)
	{
		MainUI->SetCrosshair(true);
	}
}

void APlayerCharacter::ZoomOut(const FInputActionValue& Value)
{
	bIsZooming = false;

	// 크로스헤어 숨김
	if (MainUI)
	{
		MainUI->SetCrosshair(false);
	}
}

void APlayerCharacter::UpdateCameraZoom(float DeltaTime)
{
	if (!CameraBoom)
	{
		return;
	}

	// 현재 카메라 붐 길이
	float CurrentArmLength = CameraBoom->TargetArmLength;
	
	// 타겟 길이 설정
	float TargetArmLength = bIsZooming ? ZoomedArmLength : DefaultArmLength;
	
	// 부드러운 줌 인/아웃을 위한 Lerp 적용
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomSpeed);
	
	// 카메라 붐 길이 업데이트
	CameraBoom->TargetArmLength = NewArmLength;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 카메라 줌 업데이트
	UpdateCameraZoom(DeltaTime);

	//FootStep Sound 업데이트
	UpdateFootStepSound();
}

void APlayerCharacter::UpdateFootStepSound()
{
	const FVector Velocity = GetVelocity();
	const bool bIsMoving = Velocity.SizeSquared() > 10.0f; // 아주 작을 때 제외

	//PRINTLOG(TEXT("Velocity.SizeSquared() : %f"), Velocity.SizeSquared());
	
	if (bIsMoving)
	{
		if (!footStepAudioComponent->IsPlaying())
		{
			
			footStepAudioComponent->Play();
		}
	}
	else
	{
		if (footStepAudioComponent->IsPlaying())
		{
			footStepAudioComponent->Stop();
		}
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, bIsBuilding);
	DOREPLIFETIME(APlayerCharacter, bIsInventoryOpen);
	DOREPLIFETIME(APlayerCharacter, bIsSprinting);
	DOREPLIFETIME(APlayerCharacter, SprintSpeed);
	DOREPLIFETIME(APlayerCharacter, WalkSpeed);
	DOREPLIFETIME(APlayerCharacter, PitchValue);
}

void APlayerCharacter::ServerRPC_ItemCount_Implementation(bool bIsCompleted)
{
	UE_LOG(LogTemp, Warning, TEXT("[ServerRPC_ItemCount] 791 bIsCompleted = %d"), bIsCompleted);
	MultiRPC_ItemCount(bIsCompleted);
}

void APlayerCharacter::MultiRPC_ItemCount_Implementation(bool bIsCompleted)
{
	UE_LOG(LogTemp, Warning, TEXT("[MultiRPC_ItemCount] 797 bIsCompleted = %d"), bIsCompleted);
	//모든 클라에서 호출된다.
	auto pc = Cast<APWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		auto player = pc->GetPawn<APlayerCharacter>();
		if (player && player->GoalUI)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s][MultiRPC_ItemCount] 802 bIsCompleted = %d"), GetNetMode()==NM_Client?TEXT("Client") : TEXT("Server"), bIsCompleted);
			pc->goalWidget = Cast<UGoalWidget>(player->GoalUI);
			pc->goalWidget->OnCheckMissionCompleted(bIsCompleted);
		}
	}
}

void APlayerCharacter::PlayerAttackSound(EHitType HitType)
{
	USoundBase* SelectedSound = nullptr;

	switch (HitType)
	{
	case EHitType::Air:
		SelectedSound = HitAirSound;
		break;
	case EHitType::Resource:
		SelectedSound = HitResourceSound;
		break;
	case EHitType::Pal:
		SelectedSound = HitPalSound;
		break;
	}

	if (SelectedSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			SelectedSound,
			GetActorLocation(),
			FRotator::ZeroRotator,
			1.0f,
			1.0f,
			0.0f,
			HitSoundAttenuation,
			nullptr,
			true  // bAutoDestroy
		);
	}
}

void APlayerCharacter::MultiRPC_ShowDamageText_Implementation(FVector WorldLocation, float Damage)
{
	if (DamageTextActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ADamageTextActor* DamageActor = GetWorld()->SpawnActor<ADamageTextActor>(DamageTextActorClass, WorldLocation, FRotator::ZeroRotator, SpawnParams);
		if (DamageActor)
		{
			DamageActor->SetDamageText(Damage);
		}
	}
}

void APlayerCharacter::ESCToTitle()
{
	//PRINTLOG(TEXT("[ESCToTitle] Open Widget"));

	auto pc = Cast<APlayerController>(GetController());
	if (!pc) return;

	
	if (ESCWidgetClass && !bIsESCOpened)
	{
		bIsESCOpened = true;
		ESCWidget = Cast<UESCWidget>(CreateWidget<UESCWidget>(GetWorld(), ESCWidgetClass));
		ESCWidget->AddToViewport();

		ESCWidget->PlayAnimation(ESCWidget->PopUp);

		if (ESCWidget->escSound)
		{
			UGameplayStatics::PlaySound2D(this, ESCWidget->escSound);
		}
	}
	else if (bIsESCOpened)
	{
		if (ESCWidget)
		{
			ESCWidget->SetVisibility(ESlateVisibility::Visible);
			ESCWidget->PlayAnimation(ESCWidget->PopUp);

			if (ESCWidget->escSound)
			{
				UGameplayStatics::PlaySound2D(this, ESCWidget->escSound);
			}
		}
	}

	// //입력 모드를 UI Only로 변경
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ESCWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 혹은 LockAlways
	pc->SetInputMode(InputMode);
	
	//마우스 커서 표시
	pc->bShowMouseCursor = true;
	
	
}

void APlayerCharacter::ShowItemPickupNotification(FText ItemName, UTexture2D* ItemIcon)
{
	//PRINTLOG(TEXT("22222221"));
	if (!ItemNotifyWidgetClass)
		return;
	//PRINTLOG(TEXT("33333331"));
	ItemNotifyWidget = CreateWidget<UItemNotifyWidget>(GetWorld(), ItemNotifyWidgetClass);
	if (ItemNotifyWidget)
	{
		//PRINTLOG(TEXT("11111111111111111111"));
		ItemNotifyWidget->Setup(ItemName, ItemIcon);
		ItemNotifyWidget->AddToViewport();

		//애니메이션 플레이
		ItemNotifyWidget->PlayAnimation(ItemNotifyWidget->UPAnim);
		
		// 일정 시간 후 제거
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, [&]() {
			ItemNotifyWidget->RemoveFromParent();
		}, 2.0f, false);
	}
}
