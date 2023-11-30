#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlutoPlayerCharacter.generated.h"

UCLASS()
class /*PLUTO_API*/ APlutoPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	void OnFire();

	void EnhancedInputMove(const FInputActionValue& Value);
	void EnhancedInputLook(const FInputActionValue& Value);

	void TurnAtRate(float Rate);
	void LookAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	APlutoPlayerCharacter();

	void TriggerJump();
	void StopJump();

	void TriggerCrouch();
	void StopCrouch();

	void TriggerProne();
	void StopProne();

	void TriggerSprint();
	void StopSprint();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* HandsMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* GunMesh;		

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = camera)
		class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = camera)
		float TurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = camera)
		float LookUpRate;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsStanding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsProne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsGoingForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool IsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputMappingContext* inputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToLook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToProne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputToShoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement")
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ABulletProjectile> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	class UAnimInstance* AnimInstance;

	class UWorld* World;

	FRotator SpawnRotation;
	FVector SpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health = 100.0f;

public:

	void DealDamage(float DamageAmount);
};