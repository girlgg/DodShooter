﻿#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "CommonLocalPlayer.generated.h"

UCLASS()
class COMMONGAME_API UCommonLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UCommonLocalPlayer* LocalPlayer,
	                                     APlayerController* PlayerController);
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UCommonLocalPlayer* LocalPlayer,
	                                     APlayerState* PlayerState);
	FPlayerStateSetDelegate OnPlayerStateSet;
};
