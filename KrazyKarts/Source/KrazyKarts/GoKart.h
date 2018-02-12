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

	/*
	* The MaxDrivingForce of the car in N
	*/
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90;

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


	/** Handle pressing forwards */
	void MoveForward(float Value);
	
	void MoveRight(float Value);

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime);

};
