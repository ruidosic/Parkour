// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAnimations.h"
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
	void SetAnimations(FPlayerAnimations Animations);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHanging(bool bHanging);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetClimbing(bool bClimbing);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanMoveLeft(bool bClimbingLeft);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanMoveRight(bool bClimbingRight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMoveRight(float Value);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanTurn(bool bCanTurn);
};
