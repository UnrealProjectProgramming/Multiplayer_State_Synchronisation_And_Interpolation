// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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
	FVector Velocity;

	UPROPERTY(Replicated)
	FVector ReplicatedLocation;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;



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
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);



	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime);

};
