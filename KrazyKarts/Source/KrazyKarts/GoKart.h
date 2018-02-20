// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKartMovementComponent.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	// The Velocity and the Transform and the two Canonical State that will be sent back to the Clients.
	// and the client can simulate coz we sending the last move appliad to them.
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	FVector Velocity;
	UPROPERTY()
	FGoKartMove LastMove; // it will be helpful because on the non autnomous proxy we will need that throttle to be interpulated.
};



UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	// TODO: Organise them
	void ClearAcknowledgeMoves(FGoKartMove LastMove);
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;
	UFUNCTION()
	void OnRep_ServerState();
	/** These two functions are responsible for updating the car movement Localy then calling the Server_MoveForward and MoveRight sending them the value to 
	 update them in the server, so they are both udpdate in the server and in the Clinet " Locally " */
	void MoveForward(float Value);
	void MoveRight(float Value);
	/** These two functions are responsible for updating the car movement in the server */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);
	TArray<FGoKartMove> UnacknowledgedMoves;
	
	UPROPERTY(EditAnywhere)
	UGoKartMovementComponent* GoKartMovementComponent;

};
