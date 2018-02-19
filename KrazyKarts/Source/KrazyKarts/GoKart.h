// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime; // we added the delta time so we can simulate the move on the server ( OnReciveMove ).

	UPROPERTY()
	float Time; // That is the Time at which the move started.
	// This way when we recive our LastMove from the server we can acknoldge moves and we can check if they are before or equal to that LastMove
	// and in that case we can remove them because they are Old moves. and they new moves are the one that are greater than the LastTime
	// and they are going to stay in the list.

};

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
	
	void SimulateMove(FGoKartMove Move);
	/*
	* The mass of the car in KG
	*/
	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	/*
	* The MaxDrivingForce of the car in N
	*/
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;


	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	/*
	* The MaxDrivingForce of the car in N
	*/
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	/*
	* Higher means more rolling resistance; 
	*/
	UPROPERTY(EditAnywhere)
	float RollingResistenceCoefficient = 0.015;


	UPROPERTY()
	FVector Velocity;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	float Throttle;
	float SteeringThrow;

	FVector GetAirResistance();
	FVector GetRollingResistance();

	/** These two functions are responsible for updating the car movement Localy then calling the Server_MoveForward and MoveRight sending them the value to 
	 update them in the server, so they are both udpdate in the server and in the Clinet " Locally " */
	void MoveForward(float Value);
	void MoveRight(float Value);
	/** These two functions are responsible for updating the car movement in the server */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime, float SteeringThrow);

};
