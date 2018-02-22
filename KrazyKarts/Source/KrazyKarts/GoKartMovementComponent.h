// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove& Move);


	void SetThrottle(float Val) { Throttle = Val; }
	void SetSteeringThrow(float Val) { SteeringThrow = Val; }
	void SetVelocity(FVector Val) { Velocity = Val; }
	FVector GetVelocity() { return Velocity; };
	FGoKartMove GetLastMove() { return LastMove; }
private:
	FGoKartMove CreateMove(float DeltaTime);


	FVector GetAirResistance();
	FVector GetRollingResistance();
	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime, float SteeringThrow);

	UPROPERTY()
	FVector Velocity;

	UPROPERTY(EditAnywhere)
	float Mass = 1000.0f;

	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistenceCoefficient = 0.015;

	float Throttle;
	float SteeringThrow;
	FGoKartMove LastMove;
};
