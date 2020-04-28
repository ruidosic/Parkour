// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimations.h"
#include "ParkourInterface.h"
#include "PlayerAnimInstance.generated.h"

class UAnimSequenceBase;
class UAnimMontage;



/*
	Use state machine in C++ is too suffer, so i only store variables in this class
	Main Logic complete in ABP_MainCharacter
	Nativization work properly, so its not expensive
*/



UCLASS()
class PARKOUR_API UPlayerAnimInstance : public UAnimInstance, public IParkourInterface
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	FPlayerAnimations PlayerAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bHanging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bClimbing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bCanMoveLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bCanMoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bClimbJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bCanJumpLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bCanJumpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bCanTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	float MoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsInAir;
};
