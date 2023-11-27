#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TestActor.generated.h"

UCLASS()
class ATestActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UBillboardComponent* Sprite;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Test")
	int32 Value;

public:
	ATestActor(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
};
