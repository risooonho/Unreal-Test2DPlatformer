// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2DPlatformer.h"
#include "PlatformerPlayerState.h"
#include "Net/UnrealNetwork.h"

APlatformerPlayerState::APlatformerPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* AI will remain in team 0, players are updated to team 1 through the GameMode::InitNewPlayer */
	TeamNumber = 0;
}


void APlatformerPlayerState::Reset()
{
	Super::Reset();

	NumKills = 0;
	NumDeaths = 0;
	Score = 0;
}

void APlatformerPlayerState::AddKill()
{
	NumKills++;
}

void APlatformerPlayerState::AddDeath()
{
	NumDeaths++;
}

void APlatformerPlayerState::ScorePoints(int32 Points)
{
	Score += Points;

	/* Add the score to the global score count */
    /*
	ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
	if (GS)
	{
		GS->AddScore(Points);
	}
    */
}


void APlatformerPlayerState::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}


int32 APlatformerPlayerState::GetTeamNumber() const
{
	return TeamNumber;
}

int32 APlatformerPlayerState::GetKills() const
{
	return NumKills;
}

int32 APlatformerPlayerState::GetDeaths() const
{
	return NumDeaths;
}


float APlatformerPlayerState::GetScore() const
{
	return Score;
}


void APlatformerPlayerState::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlatformerPlayerState, NumKills);
	DOREPLIFETIME(APlatformerPlayerState, NumDeaths);
	DOREPLIFETIME(APlatformerPlayerState, TeamNumber);
}


