﻿#include "UI/Widget/ScoreWidget.h"

#include "CommonNumericTextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "GameMode/DodExperienceManagerComponent.h"
#include "GameMode/GameState/TeamDeathMatchScoring.h"
#include "Kismet/GameplayStatics.h"
#include "Team/DodTeamAgentInterface.h"

void UScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UDodExperienceManagerComponent* ExperienceComponent =
		GetWorld()->GetGameState()->FindComponentByClass<UDodExperienceManagerComponent>();
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(
		FOnDodExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::HandleExperienceLoaded));
}

void UScoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateScore();
}

void UScoreWidget::HandleExperienceLoaded(const UDodExperienceDefinition* CurrentExperience)
{
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld()))
	{
		TeamDeathScoring = GameState->GetComponentByClass<UTeamDeathMatchScoring>();
	}
	if (TeamDeathScoring)
	{
		TeamDeathScoring->PlayerCounted.AddDynamic(this, &ThisClass::PlayerCounted);
		TeamDeathScoring->CountDownUpdate.AddDynamic(this, &ThisClass::CountDown);
	}
	UpdateClock(0.f);

	CurrentTeamIndex = INDEX_NONE;
	if (IDodTeamAgentInterface* TeamAgent = Cast<IDodTeamAgentInterface>(GetOwningLocalPlayer()))
	{
		TeamAgent->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnTeamChanged);
		CurrentTeamIndex = GenericTeamIdToInteger(TeamAgent->GetGenericTeamId());
	}
}

void UScoreWidget::PlayerCounted()
{
	UpdateScore();
}

void UScoreWidget::CountDown(float Time)
{
	UpdateClock(Time);
}

void UScoreWidget::OnTeamChanged(UObject* ObjectChangingTeam, int32 OldTeamID, int32 NewTeamID)
{
	CurrentTeamIndex = NewTeamID;
	if (CurrentTeamIndex != INDEX_NONE)
	{
		EnemyTeamIndex = CurrentTeamIndex == 0 ? 0 : CurrentTeamIndex == 1 ? 2 : 1;
	}
	// 更新材质
}

void UScoreWidget::UpdateScore()
{
	if (CurrentTeamIndex > 0)
	{
		UpdateAllyTeamScore();
		UpdateEnemyTeamScore();
		EvaluateSoundRoundEnding();
	}
}

void UScoreWidget::UpdateClock(float Time)
{
	int32 IntTime = Time;
	FString ClockText = FString::Printf(TEXT("%d:%d"), IntTime / 60, IntTime % 60);
	Clock->SetText(FText::FromString(ClockText));
}

void UScoreWidget::UpdateAllyTeamScore()
{
	if (!TeamDeathScoring)
	{
		return;
	}
	int32 OutCurrentScore, OutTargetScore;
	TeamDeathScoring->GetTeamScore(CurrentTeamIndex, OutCurrentScore, OutTargetScore);
	AllyScore->SetCurrentValue(OutCurrentScore);
}

void UScoreWidget::UpdateEnemyTeamScore()
{
	if (!TeamDeathScoring)
	{
		return;
	}
	int32 OutCurrentScore, OutTargetScore;
	TeamDeathScoring->GetTeamScore(EnemyTeamIndex, OutCurrentScore, OutTargetScore);
	EnemyScore->SetCurrentValue(OutCurrentScore);
}

void UScoreWidget::EvaluateSoundRoundEnding()
{
}
