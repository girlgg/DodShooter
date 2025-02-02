﻿#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTS_Shoot.generated.h"

UCLASS()
class DOD_API UBTS_Shoot : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	bool CurrentWeaponCanShoot(APawn* InPawn, AController* InController);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBlackboardKeySelector OutOfAmmo;
};
