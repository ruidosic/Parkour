// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"

DEFINE_LOG_CATEGORY(LogMainCharacter);

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(SpringArm);
}


void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ForwardTraceCheck();
	HeightTraceCheck();
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
}

void AMainCharacter::ForwardTraceCheck()
{
	FVector TraceStartPoint = GetActorLocation();
	FVector TraceEndPoint = TraceStartPoint + GetActorForwardVector() * ForwardTraceLength;

	FHitResult HitResult;
	if (GetResultFromSphereTrace(TraceStartPoint, TraceEndPoint, ForwardTraceRadius, HitResult))
	{
		WallLocation = HitResult.Location;
		WallNormal = HitResult.Normal;
	}

}

void AMainCharacter::HeightTraceCheck()
{
	FVector TraceStartPoint = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + HeightTraceElevation) + GetActorForwardVector() * HeightTraceLength;
	FVector TraceEndPoint = TraceStartPoint - FVector(0.0, 0.0, HeightTraceElevation);
	
	FHitResult HitResult;
	if (GetResultFromSphereTrace(TraceStartPoint, TraceEndPoint, HeightTraceRadius, HitResult))
	{
		HeightLocation = HitResult.Location;
	}
}

bool AMainCharacter::GetResultFromSphereTrace(FVector StartPoint, FVector EndPoint, float Radius, FHitResult& HitResult)
{
	TArray<AActor*> ActorsToIgnore;
	
	return UKismetSystemLibrary::SphereTraceSingleByProfile(this, StartPoint, EndPoint, Radius, FName("BlockAll"), false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResult, true);
}

bool AMainCharacter::IsPelvisNearLedge()
{	
	if (!GetMesh())
		UE_LOG(LogMainCharacter, Warning, TEXT("AMainCharacter::IsPelvisNearLedge -- you must set SkeletalMeshComponent for : %s "), *this->GetFullName());
	
	float DistanceBetweenPelvisAndLedge = GetMesh()->GetSocketLocation(PelvisBoneName).Z - HeightLocation.Z;

	return DistanceBetweenPelvisAndLedge >= RangeForClimb.X && DistanceBetweenPelvisAndLedge <= RangeForClimb.Y;
}

void AMainCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AMainCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

