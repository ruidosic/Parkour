// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameState.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "ParkourInterface.h"


DEFINE_LOG_CATEGORY(LogMainCharacter);

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, MoveRightInput);
	DOREPLIFETIME(AMainCharacter, WallLocation);
	DOREPLIFETIME(AMainCharacter, WallNormal);
	DOREPLIFETIME(AMainCharacter, HeightLocation);
	DOREPLIFETIME(AMainCharacter, bCanMoveLeft);
	DOREPLIFETIME(AMainCharacter, bCanMoveRight);
	DOREPLIFETIME(AMainCharacter, bCanJumpLeft);
	DOREPLIFETIME(AMainCharacter, bCanJumpRight);
}

AMainCharacter::AMainCharacter()
{
	SetReplicates(true);
	SetReplicateMovement(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

}



void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		NetUpdateFrequency = 10.0;
	}
	
	StartFrontTracesDrawTimer();
}


void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Moving
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	// Looking
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);

	// Action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::JumpClimbUp);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}


					/* Send to Server setters for replication */

void AMainCharacter::Server_SetWallNormal_Implementation(const FVector & Value)
{
	WallNormal = Value;
}

bool AMainCharacter::Server_SetWallNormal_Validate(const FVector & Value)
{
	return true;
}

void AMainCharacter::Server_SetWallLocation_Implementation(const FVector & Value)
{
	WallLocation = Value;
}

bool AMainCharacter::Server_SetWallLocation_Validate(const FVector & Value)
{
	return true;
}

void AMainCharacter::Server_SetHeightLocation_Implementation(const FVector & Value)
{
	HeightLocation = Value;
}

bool AMainCharacter::Server_SetHeightLocation_Validate(const FVector & Value)
{
	return true;
}

void AMainCharacter::Server_SetCanMoveLeft_Implementation(bool bValue)
{
	bCanMoveLeft = bValue;
}

bool AMainCharacter::Server_SetCanMoveLeft_Validate(bool bValue)
{
	return true;
}

void AMainCharacter::Server_SetCanMoveRight_Implementation(bool bValue)
{
	bCanMoveRight = bValue;
}

bool AMainCharacter::Server_SetCanMoveRight_Validate(bool bValue)
{
	return true;
}

					/* Send to AnimInstance variables */


void AMainCharacter::Server_SetCanJumpLeft_Implementation(bool bValue)
{
	bCanJumpLeft = bValue;
}

bool AMainCharacter::Server_SetCanJumpLeft_Validate(bool bValue)
{
	return true;
}

void AMainCharacter::Server_SetCanJumpRight_Implementation(bool bValue)
{
	bCanJumpRight = bValue;
}

bool AMainCharacter::Server_SetCanJumpRight_Validate(bool bValue)
{
	return true;
}

void AMainCharacter::MessegeHangingToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetHanging(AnimInstance, bHanging);
		}
	}
}

void AMainCharacter::MessegeClimbingToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetClimbing(AnimInstance, bClimbing);
		}
	}
}

void AMainCharacter::MessegeCanMoveLeftToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetCanMoveLeft(AnimInstance, bCanMoveLeft);
		}
	}
}

void AMainCharacter::MessegeCanMoveRightToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetCanMoveRight(AnimInstance, bCanMoveRight);
		}
	}
}

void AMainCharacter::MessegeMoveRightToAnim(float Value)
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetMoveRight(AnimInstance, Value);
		}
	}
}


void AMainCharacter::MessegeCanJumpLeftToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetCanJumpLeft(AnimInstance, bCanJumpLeft);
		}
	}
}

void AMainCharacter::MessegeCanJumpRightToAnim()
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetCanJumpRight(AnimInstance, bCanJumpRight);
		}
	}
}

								/* Timers Function for Server and Clients */

// Start Timer for Drawing Front Traces


void AMainCharacter::ClimbingLedgeComplete_Implementation(bool bValue)
{
	UE_LOG(LogTemp, Warning, TEXT("From Interface"))

		bClimbing = bValue;

}

float AMainCharacter::GetClimbUpMontageLength() const
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (!AnimInstance)
		{
			UE_LOG(LogMainCharacter, Warning, TEXT("AMainCharacter::SetupMontages -- you must set AnimInstance Class for: "), *GetFullName())
		}

		if (AnimInstance->Implements<UParkourInterface>())
		{
			return IParkourInterface::Execute_GetClimbUpMontageLength(AnimInstance);
		}

		if (!AnimInstance->Implements<UParkourInterface>())
		{
			UE_LOG(LogMainCharacter, Warning, TEXT("AMainCharacter::SetupAnimMontages -- cant find ParkourInterface implementation for AnimInstance: "), *AnimInstance->GetFullName())
		}
		return 0.0;
	}
	else
		return 0.0;
}

void AMainCharacter::StartFrontTracesDrawTimer()
{
	GetWorld()->GetTimerManager().SetTimer(FrontTracesDrawTimer, this, &AMainCharacter::DrawFrontTraces, FrontTracesDrawTimeInterval, true);
}


// Clear Timer for Drawing Front Traces

void AMainCharacter::ClearFrontTracesDrawTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FrontTracesDrawTimer);
}


									/* Draw Traces Functions */


void AMainCharacter::DrawFrontTraces()
{
	if (GetMovementComponent() && GetMovementComponent()->IsFalling())
	{
		ForwardTraceDraw();
		HeightTraceDraw();

		if (IsPelvisNearLedge() && !bClimbing)
		{
			ClearFrontTracesDrawTimer();
			GrabLedge();
			StartSideTracesDrawTimer();
		}
	}
}

void AMainCharacter::ForwardTraceDraw()
{
	FVector TraceStartPoint = GetActorLocation();
	FVector TraceEndPoint = TraceStartPoint + GetActorForwardVector() * ForwardTraceLength;

	FHitResult HitResult;
	if (GetResultFromSphereTrace(TraceStartPoint, TraceEndPoint, ForwardTraceRadius, HitResult))
	{
		WallLocation = HitResult.Location;
		WallNormal = HitResult.Normal;

		// Message to Server that we change wall Location and wall normal
		if (Role < ROLE_Authority)
		{
			Server_SetWallLocation(HitResult.Location);
			Server_SetWallNormal(HitResult.Normal);
		}
	}

}

void AMainCharacter::HeightTraceDraw()
{
	FVector TraceStartPoint = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + HeightTraceElevation) + GetActorForwardVector() * HeightTraceLength;
	FVector TraceEndPoint = TraceStartPoint - FVector(0.0, 0.0, HeightTraceElevation);
	
	FHitResult HitResult;
	if (GetResultFromSphereTrace(TraceStartPoint, TraceEndPoint, HeightTraceRadius, HitResult))
	{
		HeightLocation = HitResult.Location;

		// Message to Server that we change Height Location
		if (Role < ROLE_Authority)
		{
			Server_SetHeightLocation(HitResult.Location);
		}
	}
}


void AMainCharacter::DrawSideTraces()
{
	if (bHanging)
	{
		MoveLeftTraceDraw();
		MoveRightTraceDraw();
		LeftJumpTraceDraw();
		RightJumpTraceDraw();
	}

}

void AMainCharacter::MoveLeftTraceDraw()
{
	FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift +  (-1) * GetActorRightVector() * MoveSideTracesLength;
	FVector TraceEndPoint = TraceStartPoint;

	FHitResult HitResult;

	if (GetResultFromCapsuleTrace(TraceStartPoint, TraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult))
	{
		bCanMoveLeft = true;

		if (Role < ROLE_Authority)
		{
			Server_SetCanMoveLeft(true);
		}
	}
	else
	{
		bCanMoveLeft = false;

		if (Role < ROLE_Authority)
		{
			Server_SetCanMoveLeft(false);
		}
	}
}

void AMainCharacter::MoveRightTraceDraw()
{
	FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift +  GetActorRightVector() * MoveSideTracesLength;
	FVector TraceEndPoint = TraceStartPoint;

	FHitResult HitResult;

	if (GetResultFromCapsuleTrace(TraceStartPoint, TraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult))
	{
		bCanMoveRight = true;

		if (Role < ROLE_Authority)
		{
			Server_SetCanMoveRight(true);
		}
	}
	else
	{
		bCanMoveRight = false;

		if (Role < ROLE_Authority)
		{
			Server_SetCanMoveRight(false);
		}
	}
}

void AMainCharacter::LeftJumpTraceDraw()
{
	if (!bCanMoveLeft)
	{
		FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + (-1) * GetActorRightVector() * JumpSideTracesLength;
		FVector TraceEndPoint = TraceStartPoint;

		FHitResult HitResult;

		if (GetResultFromCapsuleTrace(TraceStartPoint, TraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult))
		{
			bCanJumpLeft = true;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpLeft(true);
			}
		}
		else
		{
			bCanJumpLeft = false;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpLeft(false);
			}
		}
	}
}

void AMainCharacter::RightJumpTraceDraw()
{
	if (!bCanMoveRight)
	{
		FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + GetActorRightVector() * JumpSideTracesLength;
		FVector TraceEndPoint = TraceStartPoint;

		FHitResult HitResult;

		if (GetResultFromCapsuleTrace(TraceStartPoint, TraceEndPoint, JumpSideTracesRadius, SideTracesHalfHeight, HitResult))
		{
			bCanJumpRight = true;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpRight(true);
			}
		}
		else
		{
			bCanJumpRight = false;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpRight(false);
			}
		}
	}
}

void AMainCharacter::StartSideTracesDrawTimer()
{
	GetWorld()->GetTimerManager().SetTimer(SideTracesDrawTimer, this, &AMainCharacter::DrawSideTraces, SideTracesDrawTimeInterval, true);
}

void AMainCharacter::ClearSideTracesDrawTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SideTracesDrawTimer);
}


void AMainCharacter::MoveOnLedge(float& MoveRightValue)
{

	if (!HasAuthority())
	{
		Server_MoveOnLedge(MoveRightValue);
	}
	else
	{
		Multicast_MoveOnLedge(MoveRightValue);
	}
	
	if(MoveRightValue > 0 && !bCanMoveRight || (MoveRightValue < 0 && !bCanMoveLeft))
		MoveRightValue = 0.0;

	MessegeCanMoveLeftToAnim();
	MessegeCanMoveRightToAnim();
	MessegeMoveRightToAnim(MoveRightValue);

}


void AMainCharacter::Server_MoveOnLedge_Implementation(float MoveRightValue)
{
	Multicast_MoveOnLedge(MoveRightValue);
}

void AMainCharacter::Multicast_MoveOnLedge_Implementation(float MoveRightValue)
{
	if (!IsLocallyControlled())
	{
		MessegeMoveRightToAnim(MoveRightValue);
		MessegeCanMoveLeftToAnim();
		MessegeCanMoveRightToAnim();
	}
}

bool AMainCharacter::Multicast_MoveOnLedge_Validate(float MoveRightValue)
{
	return MoveRightValue >= -1 && MoveRightValue <= 1;
}

bool AMainCharacter::Server_MoveOnLedge_Validate(float MoveRightValue)
{
	return MoveRightValue >= -1 && MoveRightValue <= 1;
}


bool AMainCharacter::GetResultFromSphereTrace(FVector StartPoint, FVector EndPoint, float Radius, FHitResult& HitResult)
{	
	return UKismetSystemLibrary::SphereTraceSingleByProfile(this, StartPoint, EndPoint, Radius, FName("BlockAll"), false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

bool AMainCharacter::GetResultFromCapsuleTrace(FVector StartPoint, FVector EndPoint, float Radius, float HalfHeight, FHitResult& HitResult)
{
	return UKismetSystemLibrary::CapsuleTraceSingleByProfile(this, StartPoint, EndPoint, Radius, HalfHeight, FName("BlockAll"), false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

void AMainCharacter::JumpLeftLedge()
{
}

void AMainCharacter::JumpRightLedge()
{
}

void AMainCharacter::JumpOnLedge(float MoveRightValue)
{
}


bool AMainCharacter::IsPelvisNearLedge()
{	
	if (!GetMesh())
	{
		UE_LOG(LogMainCharacter, Warning, TEXT("AMainCharacter::IsPelvisNearLedge -- you must set SkeletalMeshComponent for : %s "), *this->GetFullName());
		return false;
	}
	
	float DistanceBetweenPelvisAndLedge = GetMesh()->GetSocketLocation(PelvisBoneName).Z - HeightLocation.Z;

	return DistanceBetweenPelvisAndLedge >= RangeForClimb.X && DistanceBetweenPelvisAndLedge <= RangeForClimb.Y;
}


void AMainCharacter::GrabLedge()
{

	if (Role < ROLE_Authority)
		Server_GrabLedge();

	if (Role == ROLE_Authority)
		Multicast_GrabLedge();
}


void AMainCharacter::Multicast_GrabLedge_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	FLatentActionInfo Action(1, 0, TEXT("MovePlayerForHangingComplete"), this);
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), CalculateGrabLocation(), CalculateGrabRotation(), false, false, 0.13, false, EMoveComponentAction::Move, Action);
	bHanging = true;
	MessegeHangingToAnim();
}

bool AMainCharacter::Multicast_GrabLedge_Validate()
{
	return true;
}

void AMainCharacter::Server_GrabLedge_Implementation()
{
	Multicast_GrabLedge();
}

bool AMainCharacter::Server_GrabLedge_Validate()
{
	return true;
}


void AMainCharacter::ClimbingLedge()
{
	if (Role < ROLE_Authority)
	{
		Server_ClimbingLedge();
	}

	if (Role == ROLE_Authority)
	{
		Multicast_ClimbingLedge();
	}
}

void AMainCharacter::Multicast_ClimbingLedge_Implementation()
{
	if (!bClimbing)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		bClimbing = true;
		MessegeClimbingToAnim();
		bHanging = false;
	}
}

bool AMainCharacter::Multicast_ClimbingLedge_Validate()
{
	return true;
}

void AMainCharacter::Server_ClimbingLedge_Implementation()
{
	Multicast_ClimbingLedge();
}

bool AMainCharacter::Server_ClimbingLedge_Validate()
{
	return true;
}

void AMainCharacter::ReleaseLedge()
{

	if (Role < ROLE_Authority)
		Server_ReleaseLedge();

	if (Role == ROLE_Authority)
		Multicast_ReleaseLedge();
}

void AMainCharacter::ReleaseLedgeByTime(float Time)
{
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ReleaseLedge, Time, false);
}

void AMainCharacter::Multicast_ReleaseLedge_Implementation()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	WallLocation = FVector(0.0, 0.0, 0.0);
	WallNormal = FVector(0.0, 0.0, 0.0);
	HeightLocation = FVector(0.0, 0.0, 0.0);
	
	bHanging = false;
	bClimbing = false;
	
	MessegeHangingToAnim();
	MessegeClimbingToAnim();

	ClearSideTracesDrawTimer();
	StartFrontTracesDrawTimer();
}

bool AMainCharacter::Multicast_ReleaseLedge_Validate()
{
	return true;
}

void AMainCharacter::Server_ReleaseLedge_Implementation()
{
	Multicast_ReleaseLedge();
}

bool AMainCharacter::Server_ReleaseLedge_Validate()
{
	return true;
}

void AMainCharacter::MovePlayerForHangingComplete()
{
	GetMovementComponent()->StopMovementImmediately();
}


FVector AMainCharacter::CalculateGrabLocation()
{
	FVector IncreasedNormal = WallNormal * HangingDistanceByXY;
	FVector DistanceToWall = IncreasedNormal + WallLocation;
	DistanceToWall.Z = HeightLocation.Z - HangingDistanceByZ;

	return DistanceToWall;
}

FRotator AMainCharacter::CalculateGrabRotation()
{
	FRotator NewRotation = WallNormal.Rotation();

	// For look at opposite for normal direction, i.e. looking at wall 
	NewRotation.Yaw = NewRotation.Yaw - 180.0;

	return NewRotation;
}

void AMainCharacter::MoveForward(float Value)
{
	FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0, GetControlRotation().Yaw, 0.0));
	AddMovementInput(ForwardDirection, Value);

	if (Value < 0 && bHanging)
	{
		ReleaseLedge();
		StartFrontTracesDrawTimer();
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (bHanging)
	{
		MoveOnLedge(Value);
	}
	
	FVector RightDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0, GetControlRotation().Yaw, 0.0));
	AddMovementInput(RightDirection, Value);
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMainCharacter::JumpClimbUp()
{
	if (!bHanging)
	{
		Jump();
	}
	else
	{
		ClimbingLedge();
		ReleaseLedgeByTime(GetClimbUpMontageLength());
	}
}

