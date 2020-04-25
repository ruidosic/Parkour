// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ParkourPlayerController.generated.h"

 /* Returns the network-synced time from the server.
  * Corresponds to GetWorld()->GetTimeSeconds()
  * on the server. This doesn't actually make a network
  * request; it just returns the cached, locally-simulated
  * and lag-corrected ServerTime value which was synced
  * with the server at the time of this PlayerController's
  * last restart. 
  */

UCLASS()
class PARKOUR_API AParkourPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

    virtual float GetServerTime() const { return ServerTime; };
 
    virtual void ReceivedPlayer() override;

protected:

    /* Reports the current server time to clients in response to ServerRequestServerTime */

    UFUNCTION(Client, Reliable)
    void Client_ReportServerTime(float RequestWorldTime, float ServerTimeValue);

    /* Requests current server time so accurate lag compensation can be performed in ClientReportServerTime
       based on the round-trip duration */

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_RequestServerTime(APlayerController* Requester, float RequestWorldTime);

    float ServerTime = 0.0;
 
};
