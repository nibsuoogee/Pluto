#include "PlutoPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "BulletProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

#include "Log.h"

APlutoPlayerCharacter::APlutoPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);

	WalkSpeed = 600.0f;

	TurnRate = 45.0f;
	LookUpRate = 45.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));

	HandsMesh->SetOnlyOwnerSee(true);
	HandsMesh->SetupAttachment(FirstPersonCamera);
	HandsMesh->bCastDynamicShadow = false;
	HandsMesh->CastShadow = false;
	HandsMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandsMesh->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	GunMesh->SetOnlyOwnerSee(true);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	IsStanding = true;
	IsGoingForward = false;
	IsSprinting = false;

	/*
	hasJumped = false;
	hasLanded = false;

	canAct = true;
	*/
}

void APlutoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GunMesh->AttachToComponent(HandsMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("GripPoint"));

	World = GetWorld();

	AnimInstance = HandsMesh->GetAnimInstance();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(inputMappingContext, 0);
		}
	}

	CharacterMovementComponent = GetCharacterMovement();
}

void APlutoPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!IsGoingForward && IsSprinting)
	{
		StopSprint();
	}
}

void APlutoPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(inputToJump, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::TriggerJump);
		enhancedInputComponent->BindAction(inputToMove, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::EnhancedInputMove);
		enhancedInputComponent->BindAction(inputToLook, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::EnhancedInputLook);
		enhancedInputComponent->BindAction(inputToCrouch, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::TriggerCrouch);
		enhancedInputComponent->BindAction(inputToProne, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::TriggerProne);
		enhancedInputComponent->BindAction(inputToSprint, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::TriggerSprint);

		enhancedInputComponent->BindAction(inputToShoot, ETriggerEvent::Triggered, this, &APlutoPlayerCharacter::OnFire);
	}
}

void APlutoPlayerCharacter::OnFire()
{
	if(World != NULL)
	{
		SpawnRotation = GetControlRotation();

		SpawnLocation = ((MuzzleLocation != nullptr) ?
			MuzzleLocation->GetComponentLocation() :
			GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		World->SpawnActor<ABulletProjectile>(Projectile,
			SpawnLocation, SpawnRotation, ActorSpawnParams);

		if(FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		if(FireAnimation != NULL && AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.0f);
		}
	}
}

void APlutoPlayerCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void APlutoPlayerCharacter::LookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlutoPlayerCharacter::EnhancedInputMove(const FInputActionValue& Value)
{
	const FVector2D moveVector = Value.Get<FVector2D>();
	const FRotator moveRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	// movement in left or right direction
	if(moveVector.X > 0.05f || moveVector.X < -0.05f)
	{
		const FVector directionVector = moveRotation.RotateVector(FVector::RightVector);
		AddMovementInput(directionVector, moveVector.X);
	}

	// movement in forward or backward direction
	if(moveVector.Y > 0.05f || moveVector.Y < -0.05f)
	{
		const FVector directionVector = moveRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(directionVector, moveVector.Y);
	}
	IsGoingForward = moveVector.Y > 0.05f ? true : false;
}

void APlutoPlayerCharacter::EnhancedInputLook(const FInputActionValue& Value)
{
	const FVector2D lookAxisValue = Value.Get<FVector2D>();
	if(GetController())
	{
		AddControllerYawInput(lookAxisValue.X);
		AddControllerPitchInput(lookAxisValue.Y);
	}
}

void APlutoPlayerCharacter::TriggerJump()
{
	UE_LOG(LogPlutoCore, Log, TEXT("%s: on TriggerJump"), *GetName());
	StopSprint();
	ACharacter::Jump();
	/*
	if (canAct)
	{
		ACharacter::Jump();
		hasLanded = false;
		hasJumped = true;
	}
	*/
}

void APlutoPlayerCharacter::StopJump()
{
	ACharacter::StopJumping();
}

void APlutoPlayerCharacter::TriggerCrouch()
{
	if(CharacterMovementComponent)
	{
		if(!CharacterMovementComponent->IsMovingOnGround())
		{
			return;
		}
		if(CharacterMovementComponent->IsCrouching())
		{
			UE_LOG(LogPlutoCore, Log, TEXT("%s: uncrouching..."), *GetName());
			ACharacter::UnCrouch();
			CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
			IsStanding = true;
			IsSprinting = false;
			FirstPersonCamera->SetRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
		}
		else
		{
			StopSprint();
			UE_LOG(LogPlutoCore, Log, TEXT("%s: crouching..."), *GetName());
			CharacterMovementComponent->SetCrouchedHalfHeight(40.0f);
			ACharacter::Crouch();
			FirstPersonCamera->SetRelativeLocation(FVector(-39.65f, 1.75f, 36.0f));
			CharacterMovementComponent->MaxWalkSpeed = WalkSpeed/2;
			IsStanding = false;
		}
	}

}

void APlutoPlayerCharacter::TriggerProne()
{
	if(CharacterMovementComponent)
	{
		if(IsProne && CharacterMovementComponent->IsMovingOnGround())
		{
			ACharacter::UnCrouch();
			CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
			IsProne = false;
			IsStanding = true;
			IsSprinting = false;
			//GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f, true);
			FirstPersonCamera->SetRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
			UE_LOG(LogPlutoCore, Log, TEXT("%s: IsProne: %d"), *GetName(), IsProne);
			UE_LOG(LogPlutoCore, Log, TEXT("%s: UnscaledHalfHeight: %f"), *GetName(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
			UE_LOG(LogPlutoCore, Log, TEXT("%s: ScaledHalfHeight: %f"), *GetName(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		}
		else
		{
			if(IsSprinting)
			{
				UE_LOG(LogPlutoCore, Log, TEXT("%s: Should dolphin dive"), *GetName());
				FVector XYZVelocity = CharacterMovementComponent->Velocity;
				XYZVelocity.Z = 550.0f;
		        CharacterMovementComponent->Launch(XYZVelocity);
		        StopSprint();
			}
			CharacterMovementComponent->SetCrouchedHalfHeight(20.0f);
			//GetCapsuleComponent()->SetCapsuleHalfHeight(20.0f, true);
			ACharacter::Crouch();
			FirstPersonCamera->SetRelativeLocation(FVector(-39.65f, 1.75f, 20.0f));
			CharacterMovementComponent->MaxWalkSpeed = WalkSpeed*0.05;
			IsProne = true;
			IsStanding = false;
			UE_LOG(LogPlutoCore, Log, TEXT("%s: IsProne: %d"), *GetName(), IsProne);
			UE_LOG(LogPlutoCore, Log, TEXT("%s: UnscaledHalfHeight: %f"), *GetName(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
			UE_LOG(LogPlutoCore, Log, TEXT("%s: ScaledHalfHeight: %f"), *GetName(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		}
	}
}

void APlutoPlayerCharacter::TriggerSprint()
{
	if(CharacterMovementComponent)
	{
		if(!IsStanding)
		{
			TriggerCrouch();
		}
		if(IsSprinting)
		{
			CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
			IsSprinting = false;
		}
		else
		{
			CharacterMovementComponent->MaxWalkSpeed = IsGoingForward ? WalkSpeed*1.5 : WalkSpeed;
			IsSprinting = true;
		}
	}
}

void APlutoPlayerCharacter::StopSprint()
{
	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
		IsSprinting = false;
	}
}

void APlutoPlayerCharacter::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if(Health <= 0.0f)
	{
		// restart game

		
		Destroy();
	}
}