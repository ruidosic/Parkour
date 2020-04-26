// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

//void UParkourMovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//}

void UParkourMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case static_cast<int>(ECustomMovementType::MOVE_Climb) :
		{
			PhysCustomClimb(DeltaTime, Iterations);
			break;
		}
	default: break;
	}
}


void UParkourMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (!CharacterOwner)
	{
		return;
	}
}


void UParkourMovementComponent::PhysCustomClimb(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}
    RestorePreAdditiveRootMotionVelocity();

    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        if (bCheatFlying && Acceleration.IsZero())
        {
            Velocity = FVector::ZeroVector;
        }
        const float Friction = GroundFriction;
        CalcVelocity(DeltaTime, Friction, true, GetMaxBrakingDeceleration());
    }

    Iterations++;
    bJustTeleported = false;

    FVector OldLocation = UpdatedComponent->GetComponentLocation();
    const FVector Adjusted = Velocity * DeltaTime;
    FHitResult Hit(1.f);
    SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);
	
}

void UParkourMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
