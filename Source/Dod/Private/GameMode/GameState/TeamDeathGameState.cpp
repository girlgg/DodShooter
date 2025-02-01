﻿#include "GameMode/GameState/TeamDeathGameState.h"

#include "GameMode/GameState/EliminationFeedRelay.h"
#include "GameMode/GameState/TeamDeathMatchScoring.h"
#include "Messages/GameplayMessageProcessor.h"
#include "Team/DodTeamCreationComponent.h"

ATeamDeathGameState::ATeamDeathGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamDeathMatchScoring = CreateDefaultSubobject<UTeamDeathMatchScoring>(TEXT("TeamDeathMatchScoring"));
	TeamCreation = CreateDefaultSubobject<UDodTeamCreationComponent>(TEXT("TeamCreation"));
	MessageProcessor = CreateDefaultSubobject<UEliminationFeedRelay>(TEXT("MessageProcessor"));
}
