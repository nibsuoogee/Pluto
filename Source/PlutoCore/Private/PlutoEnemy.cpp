// Fill out your copyright notice in the Description page of Project Settings.


#include "PlutoEnemy.h"

#include "Components/BoxComponent.h"
#include "PlutoPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

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
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &APlutoEnemy::OnSensed);

	CurrentVelocity = FVector::ZeroVector;
	MovementSpeed = 200.0f;

	DistanceSquared = BIG_NUMBER;

}

// Called when the game starts or when spawned
void APlutoEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

				SetNewRotation(GetActorForwardVector(), GetActorLocation());
			}
		}

		SetActorLocation(NewLocation);
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
		FActorPerceptionBlueprintInfo Info;

		AIPerComp->GetActorsPerception(UpdatedActors[i], Info);
		UE_LOG(LogPlutoCore, Log, TEXT("sensed: %s"), *UpdatedActors[i]->GetClass()->GetName());
		bool is_player = false; 
		if(UpdatedActors[i]->IsA(APlutoPlayerCharacter::StaticClass()))
		{
			is_player = true; 
		}

		if(Info.LastSensedStimuli[0].WasSuccessfullySensed() && is_player)
		{
			FVector dir = UpdatedActors[i]->GetActorLocation() - GetActorLocation();
			dir.Z = 0.0f;

			CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;

			SetNewRotation(UpdatedActors[i]->GetActorLocation(), GetActorLocation());

		}
		else
		{
			FVector dir = BaseLocation - GetActorLocation();
			dir.Z = 0.0f;

			if(dir.SizeSquared2D() > 1.0f)
			{
				CurrentVelocity = dir.GetSafeNormal() * MovementSpeed;
				BackToBaseLocation = true;

				SetNewRotation(BaseLocation, GetActorLocation());
			}
		}
	}
}

void APlutoEnemy::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
	FVector NewDirection = TargetPosition - CurrentPosition;
	NewDirection.Z = 0.0f;

	EnemyRotation = NewDirection.Rotation();

	SetActorRotation(EnemyRotation);
}

void APlutoEnemy::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;

	if(Health <= 0.0f)
	{
		Destroy();
	}
}