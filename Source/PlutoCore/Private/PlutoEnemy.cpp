// Fill out your copyright notice in the Description page of Project Settings.


#include "PlutoEnemy.h"

#include "Components/BoxComponent.h"
#include "PlutoPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/KismetMathLibrary.h"

#include "Kismet/GameplayStatics.h"

#include "Log.h"

// Sets default values
APlutoEnemy::APlutoEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(RootComponent);

	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 120.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &APlutoEnemy::OnSensed);

	CurrentVelocity = FVector::ZeroVector;
	MovementSpeed = 100.0f;

	DistanceSquared = BIG_NUMBER;

}

// Called when the game starts or when spawned
void APlutoEnemy::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	
	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &APlutoEnemy::OnHit);

	BaseLocation = this->GetActorLocation();
}

// Called every frame
void APlutoEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!CurrentVelocity.IsZero())
	{
		NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;

		SetNewRotation(NewLocation, GetActorLocation(), DeltaTime);
		if(BackToBaseLocation)
		{
			if((NewLocation-BaseLocation).SizeSquared2D() < DistanceSquared)
			{
				DistanceSquared = (NewLocation - BaseLocation).SizeSquared2D();
			}
			else
			{
				CurrentVelocity = FVector::ZeroVector;
				DistanceSquared = BIG_NUMBER;
				BackToBaseLocation = false;

				
			}
		}

		SetActorLocation(NewLocation);
	}

}

void APlutoEnemy::OnFootStep()
{
	if(World != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound, GetActorLocation());
	}
}

// Called to bind functionality to input
void APlutoEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlutoEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& Hit)
{
	APlutoPlayerCharacter* Character = Cast<APlutoPlayerCharacter>(OtherActor);

	if(Character)
	{
		Character->DealDamage(DamageValue);
	}
}

void APlutoEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{
	
	for(int i = 0; i < UpdatedActors.Num(); i++)
	{
		
		if(!UpdatedActors[i]->IsA(APlutoPlayerCharacter::StaticClass()))
		{	
			continue;
		}

		FActorPerceptionBlueprintInfo Info;

		AIPerComp->GetActorsPerception(UpdatedActors[i], Info);

		if(Info.LastSensedStimuli[0].WasSuccessfullySensed())
		{
			FVector dir = UpdatedActors[i]->GetActorLocation() - GetActorLocation();
			dir.Z = 0.0f;

			CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
		}
		else
		{
			FVector dir = BaseLocation - GetActorLocation();
			dir.Z = 0.0f;

			if(dir.SizeSquared2D() > 1.0f)
			{
				CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
				BackToBaseLocation = true;
			}
		}
	}
}

void APlutoEnemy::SetNewRotation(FVector TargetPosition, FVector CurrentPosition, float DeltaTime)
{
	float InterpSpeed = 5.0f;

	FVector TargetDirection = TargetPosition - CurrentPosition;
	TargetDirection.Z = 0.0f;

	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = TargetDirection.Rotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);
	SetActorRotation(NewRotation);
}

void APlutoEnemy::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if(Health <= 0.0f)
	{
		Destroy();
	}
}