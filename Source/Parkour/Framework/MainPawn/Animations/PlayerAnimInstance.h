// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ParkourInterface.h"
#include "PlayerAnimInstance.generated.h"

class UAnimSequenceBase;
class UAnimMontage;

USTRUCT(Blueprintable)
struct FParkourAnim
{
	/*
       Because Anim without RootMotion not enabled in network game
	   And for some movement i need anims with root motion enabled
	   So i use AnimMontages, because it is only whey to do it for network game
	*/


	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* HangingIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimMontage* ClimbUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbJumpLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbJumpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbJumpUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParkourSequence")
	UAnimSequenceBase* ClimbJumpBehind;
};

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FParkourAnim ParkourAnim;

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
	float MoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient)
	bool bIsInAir;
};
