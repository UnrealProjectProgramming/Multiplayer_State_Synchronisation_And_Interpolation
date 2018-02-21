// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
private:
	void ClearAcknowledgeMoves(FGoKartMove LastMove);

	void ClientTick(float DeltaTime);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;
	UFUNCTION()
	void OnRep_ServerState();
	void AutonomousProxy_OnRep_ServerState();
	void SimulatedProxy_OnRep_ServerState();

	void UpdateServerState(FGoKartMove Move);

	/** These two functions are responsible for updating the car movement in the server */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	TArray<FGoKartMove> UnacknowledgedMoves;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdates;

	FTransform ClientStartTransform;

	UPROPERTY()
	UGoKartMovementComponent* MovementComponent;

};
