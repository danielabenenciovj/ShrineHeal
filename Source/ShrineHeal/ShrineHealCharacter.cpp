// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShrineHealCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ShrineHeal.h"
#include "HealingShrine.h"
#include "UIPlayerHUD.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AShrineHealCharacter::AShrineHealCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	MaxHealth = 100.0f;
	CurrentHealth = 50.0f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AShrineHealCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		PlayerHUD = CreateWidget<UUIPlayerHUD>(GetWorld(), PlayerHUDClass);
		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
			PlayerHUD->UpdateHealthUI(CurrentHealth, MaxHealth);
		}
	}
}

void AShrineHealCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShrineHealCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShrineHealCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShrineHealCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AShrineHealCharacter::Interact);
	}
	else
	{
		UE_LOG(LogShrineHeal, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShrineHealCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AShrineHealCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AShrineHealCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AShrineHealCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AShrineHealCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AShrineHealCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AShrineHealCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShrineHealCharacter, CurrentHealth);
	DOREPLIFETIME(AShrineHealCharacter, MaxHealth);
}

void AShrineHealCharacter::Interact(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		if (HasAuthority())
		{
			
			Server_Interact_Implementation();
		}
		else
		{
			
			Server_Interact();
		}
	}
}

// Validacion del Server RPC 
bool AShrineHealCharacter::Server_Interact_Validate()
{
	return true; 
}

// Implementacion del Server RPC
void AShrineHealCharacter::Server_Interact_Implementation()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AHealingShrine::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		AHealingShrine* Shrine = Cast<AHealingShrine>(Actor);
		if (Shrine)
		{
			Shrine->TryActivateShrine(this);
			break;
		}
	}
}

// Se ejecuta en los clientes cuando el servidor cambia CurrentHealth
void AShrineHealCharacter::OnRep_CurrentHealth()
{
	if (IsLocallyControlled())
	{
		if (IsLocallyControlled() && PlayerHUD)
		{
			PlayerHUD->UpdateHealthUI(CurrentHealth, MaxHealth);
		}
	}
}

// Implementacion del Client RPC para el mensaje privado
void AShrineHealCharacter::Client_ReceiveHealMessage_Implementation(float HealAmount)
{
	if (PlayerHUD)
	{
		PlayerHUD->ShowHealMessage(HealAmount);
	}
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Te curaste %f puntos de vida"), HealAmount);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);
	}

}
