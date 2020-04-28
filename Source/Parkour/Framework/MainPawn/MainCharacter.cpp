// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerComponents/ParkourMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "Animations/ParkourInterface.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY(LogMainCharacter);

void AMainCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainCharacter, MoveRightInput);
	DOREPLIFETIME(AMainCharacter, WallLocation);
	DOREPLIFETIME(AMainCharacter, WallNormal);
	DOREPLIFETIME(AMainCharacter, HeightLocation);
	DOREPLIFETIME(AMainCharacter, bCanMoveLeft);
	DOREPLIFETIME(AMainCharacter, bCanMoveRight);
	DOREPLIFETIME(AMainCharacter, bJumping);
	DOREPLIFETIME(AMainCharacter, bCanJumpUp);
	DOREPLIFETIME(AMainCharacter, bCanJumpLeft);
	DOREPLIFETIME(AMainCharacter, bCanJumpRight);
}

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UParkourMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	SetReplicates(true);
	SetReplicateMovement(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);

}



void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MovementComponent = Cast<UParkourMovementComponent>(GetMovementComponent());

	if (GetMesh() && GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Implements<UParkourInterface>())
	{
		IParkourInterface::Execute_SetAnimations(GetMesh()->GetAnimInstance(), Animations);
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		NetUpdateFrequency = 10.0;
	}

	// i need get camera rotation for turning back, i checked GetControlRotation and its not replicated
	Camera->SetIsReplicated(true);

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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}


/* Send to Server setters for replication */

void AMainCharacter::Server_SetWallNormal_Implementation(const FVector& Value)
{
	WallNormal = Value;
}

bool AMainCharacter::Server_SetWallNormal_Validate(const FVector& Value)
{
	return true;
}

void AMainCharacter::Server_SetWallLocation_Implementation(const FVector& Value)
{
	WallLocation = Value;
}

bool AMainCharacter::Server_SetWallLocation_Validate(const FVector& Value)
{
	return true;
}

void AMainCharacter::Server_SetHeightLocation_Implementation(const FVector& Value)
{
	HeightLocation = Value;
}

bool AMainCharacter::Server_SetHeightLocation_Validate(const FVector& Value)
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

void AMainCharacter::Server_SetIsJumping_Implementation(bool bValue)
{
	bJumping = bValue;
}

bool AMainCharacter::Server_SetIsJumping_Validate(bool bValue)
{
	return true;
}

void AMainCharacter::Server_SetCanJumpUp_Implementation(bool bValue)
{
	bCanJumpUp = bValue;
}

bool AMainCharacter::Server_SetCanJumpUp_Validate(bool bValue)
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

void AMainCharacter::JumpBackLedge()
{
	if (HasAuthority())
	{
		Multicast_JumpBackLedge();
	}
	else
	{
		Server_JumpBackLedge();
	}
}

void AMainCharacter::Server_JumpBackLedge_Implementation()
{
	Multicast_JumpBackLedge();
}

bool AMainCharacter::Server_JumpBackLedge_Validate()
{
	return true;
}

void AMainCharacter::Multicast_JumpBackLedge_Implementation()
{
	FRotator NewRotation = FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw - 180.0, GetActorRotation().Roll);
	SetActorRotation(NewRotation);
	FVector JumpDirection = GetActorForwardVector() + GetActorUpVector();
	LaunchCharacter(JumpDirection * ClimbingJumpBackStrength, true, true);
}

void AMainCharacter::MessegeCanTurnToAnim(bool bValue)
{
	if (GetMesh())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && AnimInstance->Implements<UParkourInterface>())
		{
			IParkourInterface::Execute_SetCanTurn(AnimInstance, bValue);
		}
	}
}

void AMainCharacter::Turning()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Camera->GetComponentRotation().ToString());

	if (IsTurnDirection() && !bCanTurn)
	{
		if (HasAuthority())
		{
			Multicast_Turning();
		}
		else
		{
			Server_Turning();
		}
	}
	if (!IsTurnDirection() && bCanTurn)
	{
		if (HasAuthority())
		{
			Multicast_Turning();
		}
		else
		{
			Server_Turning();
		}
	}


}

void AMainCharacter::TurningBack(bool bTurnBack)
{
	bCanTurn = bTurnBack;
	MessegeCanTurnToAnim(bTurnBack);
}

void AMainCharacter::Server_Turning_Implementation()
{
	Multicast_Turning();
}

bool AMainCharacter::Server_Turning_Validate()
{
	return true;
}

void AMainCharacter::Multicast_Turning_Implementation()
{
	if (bCanTurn)
	{
		TurningBack(false);
	}
	else
	{
		TurningBack(true);
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
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
			return Animations.ParkourAnimations.ClimbUp->SequenceLength;
	}
	else
		return 0.0;
}

float AMainCharacter::GetClimbSideJumpMontageLength() const
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		return Animations.ParkourAnimations.ClimbJumpRight->SequenceLength;
	}
	else
		return 0.0;
}

float AMainCharacter::GetClimbJumpUpMontageLength() const
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		return Animations.ParkourAnimations.ClimbJumpUp->SequenceLength;
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
	if (MovementComponent->IsFalling())
	{
		ForwardTraceDraw();

		// Height trace also check that wall in front of us
		if (HeightTracesDraw() && IsPelvisNearLedge() && !bClimbing)
		{
			ClearFrontTracesDrawTimer();
			GrabLedge();
		/*	StartSideTracesDrawTimer();*/
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

bool AMainCharacter::HeightTracesDraw()
{
	FVector HeightTraceStartPoint = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + HeightTraceElevation) + GetActorForwardVector() * HeightTraceLength;
	FVector HeightTraceEndPoint = HeightTraceStartPoint - FVector(0.0, 0.0, HeightTraceElevation);

	FVector LeftCornerTraceStartPoint = GetActorLocation() + GetActorForwardVector() * CornersCapsuleTraceForwardShift + (-1) * GetActorRightVector() * CornersCapsuleTracesLength;
	FVector LeftCornerTraceEndPoint = LeftCornerTraceStartPoint;

	FVector RightCornerTraceStartPoint = GetActorLocation() + GetActorForwardVector() * CornersCapsuleTraceForwardShift + GetActorRightVector() * CornersCapsuleTracesLength;
	FVector RightCornerTraceEndPoint = RightCornerTraceStartPoint;


	FHitResult HeightTraceHitResult;
	FHitResult TempHitResult;

	bool DetectWall = GetResultFromSphereTrace(HeightTraceStartPoint, HeightTraceEndPoint, HeightTraceRadius, HeightTraceHitResult);
	bool DetectLeftCorner = GetResultFromCapsuleTrace(LeftCornerTraceStartPoint, LeftCornerTraceEndPoint, CornesrCapsuleTraceRadius, CornersCapsuleTraceHalfHeight, TempHitResult);
	bool DetectRightCorner = GetResultFromCapsuleTrace(RightCornerTraceStartPoint, RightCornerTraceEndPoint, CornesrCapsuleTraceRadius, CornersCapsuleTraceHalfHeight, TempHitResult);

	if (DetectWall && (DetectLeftCorner ||DetectRightCorner))
	{
		HeightLocation = HeightTraceHitResult.Location;

		// Message to Server that we change Height Location
		if (Role < ROLE_Authority)
		{
			Server_SetHeightLocation(HeightTraceHitResult.Location);
		}
		return true;
	}
	return false;
}


void AMainCharacter::DrawSideTraces()
{
	MoveLeftTraceDraw();
	MoveRightTraceDraw();
	LeftJumpTraceDraw();
	RightJumpTraceDraw();
}

void AMainCharacter::MoveLeftTraceDraw()
{
	FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + (-1) * GetActorRightVector() * MoveSideTracesLength;
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
	FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + GetActorRightVector() * MoveSideTracesLength;
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

void AMainCharacter::UpJumpTraceDraw()
{
		FVector TraceStartPoint = GetActorLocation() + GetActorForwardVector() * JumpUpForwardShift + GetActorUpVector() * JumpUpTraceHeightShift;
		FVector TraceEndPoint = TraceStartPoint;

		FHitResult HitResult;

		if (GetResultFromCapsuleTrace(TraceStartPoint, TraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult))
		{
			bCanJumpUp = true;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpUp(true);
			}
		}
		else
		{
			bCanJumpUp = false;

			if (Role < ROLE_Authority)
			{
				Server_SetCanJumpUp(false);
			}
		}
}

void AMainCharacter::LeftJumpTraceDraw()
{
	if (!bCanMoveLeft)
	{
		FVector LeftCapsuleTraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + (-1) * GetActorRightVector() * JumpSideTracesLength;
		FVector LeftCapsuleTraceEndPoint = LeftCapsuleTraceStartPoint;

		FVector LeftBoxTraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + (-1) * GetActorRightVector() * JumpSideTracesLength + FVector(0.0, 0.0, SideBoxTracesHeightShift);
		FVector LeftBoxTraceEndPoint = LeftBoxTraceStartPoint;


		FHitResult HitResult;

		bool WallLeftDetect = GetResultFromCapsuleTrace(LeftCapsuleTraceStartPoint, LeftCapsuleTraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult);
		bool WallLeftUpDetect = GetResultFromBoxTrace(LeftBoxTraceStartPoint, LeftBoxTraceEndPoint, SideBoxTracesHalfSize, GetActorRotation(), HitResult);

		if (WallLeftDetect && !WallLeftUpDetect)
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
		FVector RightCapsuleTraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + GetActorRightVector() * JumpSideTracesLength;
		FVector RightCapsuleTraceEndPoint = RightCapsuleTraceStartPoint;

		FVector RightBoxTraceStartPoint = GetActorLocation() + GetActorForwardVector() * SideTracesForwardShift + GetActorRightVector() * JumpSideTracesLength + FVector(0.0, 0.0, SideBoxTracesHeightShift);
		FVector RightBoxTraceEndPoint = RightBoxTraceStartPoint;


		FHitResult HitResult;

		bool WallRightDetect = GetResultFromCapsuleTrace(RightCapsuleTraceStartPoint, RightCapsuleTraceEndPoint, MoveSideTracesRadius, SideTracesHalfHeight, HitResult);
		bool WallRightUpDetect = GetResultFromBoxTrace(RightBoxTraceStartPoint, RightBoxTraceEndPoint, SideBoxTracesHalfSize, GetActorRotation(), HitResult);

		if (WallRightDetect && !WallRightUpDetect)
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


void AMainCharacter::MoveOnLedge(float MoveRightValue)
{
	if (!HasAuthority())
	{
		Server_MoveOnLedge(MoveRightValue);
	}
	else
	{
		Multicast_MoveOnLedge(MoveRightValue);
	}

	if (MoveRightValue > 0 && !bCanMoveRight || (MoveRightValue < 0 && !bCanMoveLeft))
		MoveRightValue = 0.0;

	AddMovementInput(GetActorRightVector(), MoveRightValue);

	MessegeCanMoveLeftToAnim();
	MessegeCanMoveRightToAnim();
	MessegeMoveRightToAnim(MoveRightValue);

}


void AMainCharacter::Server_MoveOnLedge_Implementation(float MoveRightValue)
{
	Multicast_MoveOnLedge(MoveRightValue);
	MessegeMoveRightToAnim(MoveRightValue);
	MessegeCanMoveLeftToAnim();
	MessegeCanMoveRightToAnim();
}

void AMainCharacter::Multicast_MoveOnLedge_Implementation(float MoveRightValue)
{
	if (!IsLocallyControlled() && !HasAuthority())
	{
		MessegeMoveRightToAnim(MoveRightValue);
		MessegeCanMoveLeftToAnim();
		MessegeCanMoveRightToAnim();
	}
}


bool AMainCharacter::Server_MoveOnLedge_Validate(float MoveRightValue)
{
	return MoveRightValue >= -1 && MoveRightValue <= 1;
}


bool AMainCharacter::GetResultFromSphereTrace(const FVector StartPoint, const FVector EndPoint, const float Radius, FHitResult& HitResult)
{
	return UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPoint, EndPoint, Radius, ClimbingObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

bool AMainCharacter::GetResultFromCapsuleTrace(const FVector StartPoint, const FVector EndPoint, const float Radius, const float HalfHeight, FHitResult& HitResult)
{
	return UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, StartPoint, EndPoint, Radius, HalfHeight, ClimbingObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

bool AMainCharacter::GetResultFromBoxTrace(const FVector StartPoint, const FVector EndPoint, const FVector HalfSize, const FRotator Orientation, FHitResult& HitResult)
{
	return UKismetSystemLibrary::BoxTraceSingleForObjects(this, StartPoint, EndPoint, HalfSize, Orientation, ClimbingObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

void AMainCharacter::JumpLeftLedge()
{
	bJumping = true;

	DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbLeftJumpingComplete, GetClimbSideJumpMontageLength(), false);
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpLeft);
	}

	if (Role < ROLE_Authority)
	{
		Server_JumpLeftLedge();
	}
	if(Role == ROLE_Authority)
	{
		Multicast_JumpLeftLedge();
	}
}

void AMainCharacter::Server_JumpLeftLedge_Implementation()
{
	Multicast_JumpLeftLedge();
}

bool AMainCharacter::Server_JumpLeftLedge_Validate()
{
	return true;
}

void AMainCharacter::Multicast_JumpLeftLedge_Implementation()
{
	if (!IsLocallyControlled())
	{
		bJumping = true;

		DisableInput(UGameplayStatics::GetPlayerController(this, 0));
		
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbLeftJumpingComplete, GetClimbSideJumpMontageLength(), false);
		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpLeft);
		}
	}
}

void AMainCharacter::JumpRightLedge()
{
	bJumping = true;

	DisableInput(UGameplayStatics::GetPlayerController(this, 0));
	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbRightJumpingComplete, GetClimbSideJumpMontageLength(), false);
	
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpRight);
	}

	if (Role < ROLE_Authority)
	{
		Server_JumpRightLedge();
	}
	if (Role == ROLE_Authority)
	{
		Multicast_JumpRightLedge();
	}
}


void AMainCharacter::Server_JumpRightLedge_Implementation()
{
	Multicast_JumpRightLedge();
}

bool AMainCharacter::Server_JumpRightLedge_Validate()
{
	return true;
}

void AMainCharacter::Multicast_JumpRightLedge_Implementation()
{
	if (!IsLocallyControlled())
	{
		bJumping = true;

		DisableInput(UGameplayStatics::GetPlayerController(this, 0));

		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbRightJumpingComplete, GetClimbSideJumpMontageLength(), false);

		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpRight);
		}
		
	}
}


void AMainCharacter::JumpUpLedge()
{
	bJumping = true;

	DisableInput(UGameplayStatics::GetPlayerController(this, 0));

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbUpJumpingComplete, GetClimbJumpUpMontageLength(), false);

	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpUp);
	}

	if (Role < ROLE_Authority)
	{
		Server_JumpUpLedge();
	}
	if (Role == ROLE_Authority)
	{
		Multicast_JumpUpLedge();
	}
}



void AMainCharacter::Server_JumpUpLedge_Implementation()
{
	Multicast_JumpUpLedge();
}

bool AMainCharacter::Server_JumpUpLedge_Validate()
{
	return true;
}

void AMainCharacter::Multicast_JumpUpLedge_Implementation()
{
	if (!IsLocallyControlled())
	{
		bJumping = true;

		DisableInput(UGameplayStatics::GetPlayerController(this, 0));

		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ClimbUpJumpingComplete, GetClimbJumpUpMontageLength(), false);

		if (GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbJumpUp);
		}
	}
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

void AMainCharacter::GrabLedgeByTime(float Time)
{
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::GrabLedge, Time, false);
}


void AMainCharacter::Multicast_GrabLedge_Implementation()
{
	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
	MovementComponent->bOrientRotationToMovement = false;
	FLatentActionInfo Action(1, 0, TEXT("MovePlayerForHangingComplete"), this);
	UKismetSystemLibrary::MoveComponentTo(GetCapsuleComponent(), CalculateGrabLocation(), CalculateGrabRotation(), false, false, 0.13, false, EMoveComponentAction::Move, Action);
	bHanging = true;
	MessegeHangingToAnim();
	MessegeMoveRightToAnim(MoveRightInput);
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
		bClimbing = true;
		MessegeClimbingToAnim();
		bHanging = false;
		GetMesh()->GetAnimInstance()->Montage_Play(Animations.ParkourAnimations.ClimbUp);
	}
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

	MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	MovementComponent->bOrientRotationToMovement = true;

	WallLocation = FVector(0.0, 0.0, 0.0);
	WallNormal = FVector(0.0, 0.0, 0.0);
	HeightLocation = FVector(0.0, 0.0, 0.0);

	bHanging = false;
	bClimbing = false;

	MessegeHangingToAnim();
	MessegeClimbingToAnim();

	StartFrontTracesDrawTimer();
}

void AMainCharacter::ReleaseLedgeByTime(float Time)
{
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMainCharacter::ReleaseLedge, Time, false);
}

void AMainCharacter::Multicast_ReleaseLedge_Implementation()
{
	if (!IsLocallyControlled())
	{
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
		MovementComponent->bOrientRotationToMovement = true;

		WallLocation = FVector(0.0, 0.0, 0.0);
		WallNormal = FVector(0.0, 0.0, 0.0);
		HeightLocation = FVector(0.0, 0.0, 0.0);

		bHanging = false;
		bClimbing = false;

		MessegeHangingToAnim();
		MessegeClimbingToAnim();

		StartFrontTracesDrawTimer();
	}

}


void AMainCharacter::Server_ReleaseLedge_Implementation()
{
	Multicast_ReleaseLedge();

	WallLocation = FVector(0.0, 0.0, 0.0);
	WallNormal = FVector(0.0, 0.0, 0.0);
	HeightLocation = FVector(0.0, 0.0, 0.0);

	bHanging = false;
	bClimbing = false;
}

bool AMainCharacter::Server_ReleaseLedge_Validate()
{
	return true;
}

void AMainCharacter::MovePlayerForHangingComplete()
{
	MovementComponent->SetMovementMode(EMovementMode::MOVE_Custom, static_cast<int>(ECustomMovementType::MOVE_Climb));
	MovementComponent->StopMovementImmediately();
	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
}

void AMainCharacter::ClimbLeftJumpingComplete()
{
	bJumping = false;
	Server_SetIsJumping(bJumping);

	bCanJumpLeft = false;
	Server_SetCanJumpLeft(bCanJumpLeft);

	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
}

void AMainCharacter::ClimbRightJumpingComplete()
{
	bJumping = false;
	Server_SetIsJumping(bJumping);

	bCanJumpRight = false;
	Server_SetCanJumpRight(bCanJumpRight);

	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
}

void AMainCharacter::ClimbUpJumpingComplete()
{
	bJumping = false;
	Server_SetIsJumping(bJumping);

	bCanJumpUp = false;
	Server_SetCanJumpUp(bCanJumpUp);

	EnableInput(UGameplayStatics::GetPlayerController(this, 0));

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

bool AMainCharacter::IsTurnDirection()
{
	FVector ControlDirection = UKismetMathLibrary::GetForwardVector(Camera->GetComponentRotation());
	FVector PlayerDirection = UKismetMathLibrary::GetForwardVector(GetActorRotation());
	float DotProduct = ControlDirection.X * PlayerDirection.X + ControlDirection.Y * PlayerDirection.Y + ControlDirection.Z * PlayerDirection.Z;
	return DotProduct < 0;
	
}

void AMainCharacter::MoveForward(float Value)
{
	MoveForwardInput = Value;

	if (!bHanging)
	{
		FVector ForwardDirection = UKismetMathLibrary::GetForwardVector(FRotator(0.0, GetControlRotation().Yaw, 0.0));
		AddMovementInput(ForwardDirection, Value * ClimbingMovementSpeedMultiplier);
	}

	if (Value < 0 && bHanging)
	{
		ReleaseLedge();
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (bHanging)
	{
		DrawSideTraces();

		MoveOnLedge(Value);
	}
	if(!bHanging)
	{
		FVector RightDirection = UKismetMathLibrary::GetRightVector(FRotator(0.0, GetControlRotation().Yaw, 0.0));
		AddMovementInput(RightDirection, Value);
	}

	MoveRightInput = Value;
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);

	// Do Once
	if (bHanging)
	{
		Turning();
	}

}

void AMainCharacter::JumpPressed()
{
	if (!bHanging)
	{
		Jump();
	}

	// its trace draw do check for bCanJumpUp 
	if  (bHanging && !IsTurnDirection())
	{
		UpJumpTraceDraw();
	}

	if (bHanging && IsTurnDirection())
	{
		ReleaseLedge();
		JumpBackLedge();
	}

	// Climbing Up
	if (bHanging && MoveRightInput == 0.0 && !bCanJumpUp && !IsTurnDirection())
	{
		ClimbingLedge();
		ReleaseLedgeByTime(GetClimbUpMontageLength());
	}

	// Jump Up
	if(bHanging && MoveRightInput == 0.0 && bCanJumpUp && !IsTurnDirection())
	{
		JumpUpLedge();
	}

	if (bCanJumpLeft && MoveRightInput < 0 && !IsTurnDirection())
	{
		JumpLeftLedge();
	}

	if (bCanJumpRight && MoveRightInput > 0 && !IsTurnDirection())
	{
		JumpRightLedge();
	}


}


