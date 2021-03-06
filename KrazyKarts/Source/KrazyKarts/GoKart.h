// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGoKartMovementReplicator* MovementReplicator;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	// TODO: Organise them
	/** These two functions are responsible for updating the car movement Localy then calling the Server_MoveForward and MoveRight sending them the value to 
	 update them in the server, so they are both udpdate in the server and in the Clinet " Locally " */
	void MoveForward(float Value);
	void MoveRight(float Value);
	

	UPROPERTY(VisibleAnywhere)
	UGoKartMovementComponent* MovementComponent;

};
