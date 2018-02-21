// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKartMovementReplicator.h"

#include "UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicated(true);
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();
	
	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();

}


// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!ensure(MovementComponent != nullptr)) { return; }
	FGoKartMove LastMove = MovementComponent->GetLastMove();
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}
	// This means that we are the server and in controll of the Pawn.
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UpdateServerState(LastMove);
	}
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
	}
}

void UGoKartMovementReplicator::UpdateServerState(FGoKartMove Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

void UGoKartMovementReplicator::ClientTick(float DeltaTime)
{
	// PsudoCode for linear Interpolation
	// TargetLocation = ServerState.Location
	// Lerp ratio = TimeScienceUpdate / TimeBetweenLastUpdates
	// NextLocation = Lerp ( StartLocation, TargetLocation , Lerp ratio)
	// SetLocation (NextLocation)

	ClientTimeSinceUpdate += DeltaTime;
	if (ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER) { return; }

	auto TargetLocation = ServerState.Transform.GetLocation();
	auto TargetRotation = ServerState.Transform.GetRotation();

	float LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;

	FVector StartLocation = ClientStartTransform.GetLocation();
	FQuat StartRotation = ClientStartTransform.GetRotation();

	auto NewLocation = FMath::LerpStable(StartLocation, TargetLocation, LerpRatio);
	auto NewRotation = FQuat::Slerp(StartRotation, TargetRotation, LerpRatio);

	GetOwner()->SetActorLocation(NewLocation);
	GetOwner()->SetActorRotation(NewRotation);
}

void UGoKartMovementReplicator::ClearAcknowledgeMoves(FGoKartMove LastMove)
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

void UGoKartMovementReplicator::OnRep_ServerState()
{
	switch (GetOwnerRole())
	{
	case ROLE_AutonomousProxy:
		AutonomousProxy_OnRep_ServerState();
		break;
	case ROLE_SimulatedProxy:
		SimulatedProxy_OnRep_ServerState();
		break;
	default:
		break;
	}
}

void UGoKartMovementReplicator::AutonomousProxy_OnRep_ServerState()
{
	if (!ensure(MovementComponent != nullptr)) { return; }
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);
	ClearAcknowledgeMoves(ServerState.LastMove);
	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UGoKartMovementReplicator::SimulatedProxy_OnRep_ServerState()
{
	// PsudoCode for linear Interpolation
	// StartLocation = GetLocation()

	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;

	ClientStartTransform = GetOwner()->GetActorTransform();
}


void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	if (!ensure(MovementComponent != nullptr)) { return; }

	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	bool bIsValid = (Move.Throttle <= 1 && Move.Throttle >= -1 && Move.SteeringThrow <= 1 && Move.SteeringThrow >= -1);
	return bIsValid;
}


