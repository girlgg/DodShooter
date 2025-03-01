﻿#include "AbilitySystem/Abilityies/DodGameplayAbility_ADS.h"

#include "DodGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/UIMessaging.h"
#include "Player/DodPlayerController.h"

bool UDodGameplayAbility_ADS::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayTagContainer* SourceTags,
                                                 const FGameplayTagContainer* TargetTags,
                                                 FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UDodGameplayAbility_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// TODO 设置相机（相机组件）
	// TODO 设置速度（移动|速度组件）
	// TODO 应用ADS时输出上下文映射（更高优先级）
	if (IsLocallyControlled())
	{
		// UI设置
		BroadcastToUI(true);
		// 播放ADS音效
	}
}

void UDodGameplayAbility_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	// TODO 恢复相机
	if (IsLocallyControlled())
	{
		BroadcastToUI(false);
		// 播放音效
	}
	// TODO 设置速度
	// 移除上下文映射
}

void UDodGameplayAbility_ADS::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UDodGameplayAbility_ADS::BroadcastToUI(bool bInOn)
{
	FUIMessaging MessageUI;
	MessageUI.bOn = bInOn;
	MessageUI.Controller = Cast<APlayerController>(GetDodPlayerControllerFromActorInfo());
	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(DodGameplayTags::Gameplay_Message_ADS, MessageUI);
}
