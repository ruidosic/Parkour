// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/BlendSpaceBase.h" 
#include "PlayerAnimations.generated.h"



USTRUCT(Blueprintable)
struct FMovementAnimations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UAnimSequenceBase* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UBlendSpaceBase* LocomotionStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UBlendSpaceBase* Locomotion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UAnimSequenceBase* JumpStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UAnimSequenceBase* JumpLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	UAnimSequenceBase* JumpEnd;
};


USTRUCT(Blueprintable)
struct FParkourAnimations
{
	/*
	   Because Anim without RootMotion not enabled in network game
	   And for some movement i need anims with root motion enabled
	   So i use AnimMontages, because it is only whey to do it for network game
	*/


	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimSequenceBase* HangingIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimMontage* ClimbUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimSequenceBase* TurnBehind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimSequenceBase* ClimbLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimSequenceBase* ClimbRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimMontage* ClimbJumpLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimMontage* ClimbJumpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	UAnimMontage* ClimbJumpUp;

};




USTRUCT(Blueprintable)
struct FPlayerAnimations 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementAnimations")
	FMovementAnimations MovementAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourAnimations")
	FParkourAnimations ParkourAnimations;

};
