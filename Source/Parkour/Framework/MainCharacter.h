// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMainCharacter, Display, All);

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PARKOUR_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMainCharacter();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|ForwardTrace")
	float ForwardTraceLength = 150.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|ForwardTrace")
	float ForwardTraceRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|HeightTrace")
	float HeightTraceElevation = 500.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|HeightTrace")
	float HeightTraceRadius = 20.0;

	UPROPERTY(EditAnywhere, Category = "TraceSettings|HeightTrace")
	float HeightTraceLength = 70.0;

	/* We must know this bone name for calculating distance between ledge and pelvis */
	UPROPERTY(EditAnywhere)
	FName PelvisBoneName = FName("Pelvis");

	UPROPERTY(EditAnywhere)
	FVector2D RangeForClimb = FVector2D(-50, 0.0);

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCameraComponent* Camera;

	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	void ForwardTraceCheck();
	FVector WallLocation;
	FVector WallNormal;
	
	void HeightTraceCheck();
	FVector HeightLocation;

	bool GetResultFromSphereTrace(FVector StartPoint, FVector EndPoint, float Radius, FHitResult& HitResult);

	bool IsPelvisNearLedge();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);
};
