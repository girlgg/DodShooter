﻿#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "DodGameViewportClient.generated.h"

UCLASS()
class DOD_API UDodGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UDodGameViewportClient(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
	                  bool bCreateNewAudioDevice = true) override;
};
