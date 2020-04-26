// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourInterface.generated.h"



/*
	  Interface realised into PlayerAnimInstance
	  So we send data to AnimInstance from this interface
	  I know that it was bad name for interface
*/

class UAnimMontage;

UINTERFACE(MinimalAPI)
class UParkourInterface : public UInterface
{
	GENERATED_BODY()
};



class PARKOUR_API IParkourInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHanging(bool bHanging);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetClimbing(bool bClimbing);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanMoveLeft(bool bClimbingLeft);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanJumpUp(bool bCanJumpLeft);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanMoveRight(bool bClimbingRight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanJumpLeft(bool bCanJumpLeft);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetIsJumping(bool bClimbJumping);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanJumpRight(bool bCanJumpRight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMoveRight(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float GetClimbUpMontageLength() const;

	// side jump animations must have equal length,
	//because if it is not, then movement wasnt sync 
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float GetClimbSideJumpMontageLength() const;
};
