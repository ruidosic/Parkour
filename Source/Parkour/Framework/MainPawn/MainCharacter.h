// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UParkourMovementComponent;
class UAnimInstance;
class UAnimMontage;

DECLARE_LOG_CATEGORY_EXTERN(LogMainCharacter, Display, All);



UCLASS()
class PARKOUR_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMainCharacter(const FObjectInitializer& ObjectInitializer);


	// For Initialize ParkourMovementComponent
	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

	/* Configuration Interface (Maybe need wrap this into model/struct ) */
	
	UPROPERTY(EditAnywhere, Category = "CommonMovement")
	UAnimMontage* TestMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMovement")
	float CharacterWalkingSpeed = 200;

	UPROPERTY(EditAnywhere, Category = "CommonMovement")
	float CharacterRunningSpeed = 300;

	UPROPERTY(EditAnywhere, Category = "ClimbingMovements", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ClimbingMovementSpeedMultiplier = 1.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|Jumping", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ClimbingSideJumpLength = 150.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces")
	float FrontTracesDrawTimeInterval = 0.01;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces|ForwardTrace")
	float ForwardTraceLength = 150.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces|ForwardTrace")
	float ForwardTraceRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces|HeightTrace")
	float HeightTraceElevation = 500.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces|HeightTrace")
	float HeightTraceRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|FrontTraces|HeightTrace")
	float HeightTraceLength = 70.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpUpTrace")
	float JumpUpTraceRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpUpTrace")
	float JumpUpTraceHalfHeight = 100.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpUpTrace")
	float JumpUpTraceHeightShift = 200.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpUpTrace")
	float JumpUpForwardShift = 70.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|MoveLeftRightTraces")
	float MoveSideTracesLength = 70.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|MoveLeftRightTraces")
	float MoveSideTracesRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpLeftRightTraces")
	float JumpSideTracesLength = 110.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpLeftRightTraces")
	float JumpSideTracesRadius = 25.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpLeftRightTraces")
	float SideTracesForwardShift = 40.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|JumpLeftRightTraces")
	float SideTracesHalfHeight = 60.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|Hanging")
	float HangingDistanceByXY = 25.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|Hanging")
	float HangingDistanceByZ = 135.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|Hanging")
	FVector2D RangeForClimb = FVector2D(-50, 0.0);

	UPROPERTY(EditAnywhere, Category = "TraceSettings")
	TArray<AActor*> ActorsToIgnore;


	/* We must know this bone name for calculating distance between ledge and pelvis */
	UPROPERTY(EditAnywhere, Category = "TraceSettings")
	FName PelvisBoneName = FName("Pelvis");

	// Need to be BP Callable for Latent Action 
	// Function was called after ComponentMoveTo action complete 
	UFUNCTION(BlueprintCallable)
	void MovePlayerForHangingComplete();

	UFUNCTION(BlueprintCallable)
	void ClimbLeftJumpingComplete();

	UFUNCTION(BlueprintCallable)
	void ClimbRightJumpingComplete();

public:	

	/* Cameras Components */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;


	// maybe we want get this later
	UFUNCTION(BlueprintCallable, Category = "MovementComponent")
	FORCEINLINE UParkourMovementComponent* GetParkourMovementComponent() const { return MovementComponent; }

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

			/*  Calculations Variables, that sends to the server for Sync */

	/* Wall detection Variables */

	UFUNCTION(Server,Reliable, WithValidation)
	void Server_SetWallNormal(const FVector& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetWallLocation(const FVector& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetHeightLocation(const FVector& Value);

	/* Side Movement */

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetCanMoveLeft(bool bValue);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SetCanMoveRight(bool bValue);

	/* Jumps */

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetIsJumping(bool bValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanJumpLeft(bool bValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanJumpRight(bool bValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCanJumpUp(bool bValue);

	/* Grabing Ledge Functions and variables */ 

	bool bHanging;

	void MessegeHangingToAnim();

	void GrabLedge();
	void GrabLedgeByTime(float Time);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GrabLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_GrabLedge();

	void ReleaseLedge();
	void ReleaseLedgeByTime(float Time);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReleaseLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ReleaseLedge();

	/* Climbing Ledge Functions and variables */

	bool bClimbing;

	void MessegeClimbingToAnim();

	void ClimbingLedge();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ClimbingLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ClimbingLedge();

	void ClimbingLedgeComplete_Implementation(bool bValue);

private:

	/* we declare custom movement mode (MOVE_Climb) in this movement component  */
	UParkourMovementComponent* MovementComponent;

	/* Anim Montages from AnimInstance*/

	float GetClimbUpMontageLength() const;
	float GetClimbSideJumpMontageLength() const;

				/* Drawing Traces */

	/* Front Traces thats draw every time when Character jumping */

	void StartFrontTracesDrawTimer();

	void ClearFrontTracesDrawTimer();
	
	FTimerHandle FrontTracesDrawTimer;

	UFUNCTION()
	void DrawFrontTraces();
	void ForwardTraceDraw();
	bool HeightTraceDraw();

			/* Side Traces and Side Movement  */

	/* Side Traces */

	UFUNCTION()
	void DrawSideTraces();
	
	/* Side Move Traces */
	
	void MoveLeftTraceDraw();
	void MoveRightTraceDraw();

	/* Side Jump Traces */

	void UpJumpTraceDraw();
	void LeftJumpTraceDraw();
	void RightJumpTraceDraw();

	/* Side Movement */

	void MessegeCanMoveLeftToAnim();
	void MessegeCanMoveRightToAnim();
	void MessegeMoveRightToAnim(float Value);

	void MoveOnLedge(float MoveRightValue);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_MoveOnLedge(float MoveRightValue);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_MoveOnLedge(float MoveRightValue);

	/* Jump Movements */

	void MessegeCanJumpLeftToAnim();
	void MessegeCanJumpRightToAnim();
	void MessegeCanJumpUpToAnim();
	void MessegeIsJumpingToAnim();

	void JumpLeftLedge();

	UFUNCTION(Server, Reliable, WithValidation)
    void Server_JumpLeftLedge();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpLeftLedge();

	void JumpRightLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_JumpRightLedge();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpRightLedge();

	void JumpUpLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_JumpUpLedge();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpUpLedge();

	// Mesc for addition Calculating and replication
	float MoveForwardInput;
	
	UPROPERTY(Replicated, Transient)
	float MoveRightInput;

	UPROPERTY(Replicated, Transient)
	FVector WallLocation;

	UPROPERTY(Replicated, Transient)
	FVector WallNormal;

	UPROPERTY(Replicated, Transient)
	FVector HeightLocation;

	UPROPERTY(Replicated, Transient)
	bool bCanMoveLeft;

	UPROPERTY(Replicated, Transient)
	bool bCanMoveRight;

	UPROPERTY(Replicated, Transient)
	bool bJumping;

	UPROPERTY(Replicated, Transient)
	bool bCanJumpUp;

	UPROPERTY(Replicated, Transient)
	bool bCanJumpLeft;

	UPROPERTY(Replicated, Transient)
	bool bCanJumpRight;


	/* Calcucation Functions */

	bool IsPelvisNearLedge();
	FVector CalculateGrabLocation();
	FRotator CalculateGrabRotation();
	bool GetResultFromSphereTrace(FVector StartPoint, FVector EndPoint, float Radius, FHitResult& HitResult);
	bool GetResultFromCapsuleTrace(FVector StartPoint, FVector EndPoint, float Radius, float HalfHeight, FHitResult& HitResult);


	/* Player's Input Functions */

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
	void JumpPressed();
	void Test();
};
