// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECustomMovementType : uint8
{
	MOVE_Climb UMETA(DisplayName = "Climbing")
};


UCLASS()
class PARKOUR_API UParkourMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:

    virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;

    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
    void PhysCustomClimb(float DeltaTime, int32 Iterations);

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
