// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"

#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

#include "GoKartMovementComponent.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	GoKartMovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementComponent"));

}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ServerState);
}


FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "failed to return an enum";
		break;
	}
}
// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!ensure(GoKartMovementComponent != nullptr)) { return; }

	if (Role == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = GoKartMovementComponent->CreateMove(DeltaTime);
		GoKartMovementComponent->SimulateMove(Move);
		UnacknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}
	if (Role == ROLE_SimulatedProxy)
	{
		GoKartMovementComponent->SimulateMove(ServerState.LastMove);
	}
	// This means that we are the server and in controll of the Pawn.
	if (Role == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy)
	{
		FGoKartMove Move = GoKartMovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100.0f), GetEnumText(Role), this, FColor::White, DeltaTime);
}



void AGoKart::ClearAcknowledgeMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}
	UnacknowledgedMoves = NewMoves;
}

void AGoKart::OnRep_ServerState()
{
	if (!ensure(GoKartMovementComponent != nullptr)) { return; }

	SetActorTransform(ServerState.Transform);
	GoKartMovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgeMoves(ServerState.LastMove);
	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		GoKartMovementComponent->SimulateMove(Move);
	}
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}


void AGoKart::MoveForward(float Value)
{
	if (!ensure(GoKartMovementComponent != nullptr)) { return; }
	GoKartMovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value)
{
	if (!ensure(GoKartMovementComponent != nullptr)) { return; }
	GoKartMovementComponent->SetSteeringThrow(Value);
}

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (!ensure(GoKartMovementComponent != nullptr)) { return; }

	GoKartMovementComponent->SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = GoKartMovementComponent->GetVelocity();
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	bool bIsValid = ( Move.Throttle <= 1 && Move.Throttle >= -1  && Move.SteeringThrow <= 1 && Move.SteeringThrow >= -1);
	return bIsValid;
}

