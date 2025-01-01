﻿#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DodAnimInstance.generated.h"

struct FGameplayTag;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	BlendOut,
	Hold,
	Accumulate,
	Max UMETA(Hidden)
};

UCLASS()
class DOD_API UDodAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	//~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	//~ End of UAnimInstance interface

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	void OnGameplayTagAdsChanged(const FGameplayTag Tag, int32 NewCount);
	void OnGameplayTagFiringChanged(const FGameplayTag Tag, int32 NewCount);
	void OnGameplayTagReloadingChanged(const FGameplayTag Tag, int32 NewCount);
	void OnGameplayTagMeleeChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	void UpdateLocationData(float DeltaSeconds);
	void UpdateRotationData(float DeltaSeconds);
	void UpdateVelocityData(float DeltaSeconds);
	void UpdateAccelerationData(float DeltaSeconds);
	void UpdateWallDetectionHeuristic();
	void UpdateCharacterStateData(float DeltaSeconds);
	void UpdateBlendWeightData(float DeltaSeconds);
	void UpdateRootYawOffset(float DeltaSeconds);
	void UpdateAimingData();
	void UpdateJumpFallData();

	void SetRootYawOffset(float InRootYawOffset);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnInPlace")
	float RootYawOffset{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationData")
	FVector WorldLocation{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationData")
	float DisplacementSinceLastUpdate{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LocationData")
	float DisplacementSpeed{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RotationData")
	FRotator WorldRotation{FRotator::ZeroRotator};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RotationData")
	float YawDeltaSinceLastUpdate{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RotationData")
	float YawDeltaSpeed{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	FVector LocalVelocity2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	FVector LocalDirection2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	FVector WorldVelocity{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	FVector WorldVelocity2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	float LocalVelocityDirectionAngle{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	float LocalVelocityDirectionAngleWithOffset{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AccelerationData")
	float TimeSinceFiredWeapon{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AccelerationData")
	FVector WorldAcceleration2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AccelerationData")
	FVector LocalAcceleration2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AccelerationData")
	FVector PivotDirection2D{FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BlendWeightData")
	float UpperBodyDynamicAdditiveWeight{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AimingData")
	float AimPitch{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AimingData")
	float AimYaw{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	float TimeToJumpApex{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnInPlace")
	FVector2D RootYawOffsetAngleClamp{-120.f, 100.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnInPlace")
	FVector2D RootYawOffsetAngleClampCrouched{-90.f, 80.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnInPlace")
	ERootYawOffsetMode RootYawOffsetMode{ERootYawOffsetMode::BlendOut};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurnInPlace")
	uint8 bEnableRootYawOffset : 1{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AccelerationData")
	uint8 bHasAcceleration : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bIsRunningIntoWall : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bIsOnGround : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bIsCrouching : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bCrouchStateChanged : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bWasCrouchingLastUpdate : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bWasAdsLastUpdate : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bAdsStateChanged : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bIsJumping : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CharacterStateData")
	uint8 bIsFalling : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	uint8 bHasVelocity : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VelocityData")
	uint8 bWasMovingLastUpdate : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponInfo")
	uint8 bIsAds : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponInfo")
	uint8 bIsFiring : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponInfo")
	uint8 bIsReloading : 1{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponInfo")
	uint8 bIsMelee : 1{false};

private:
	uint8 bIsFirstUpdate : 1{true};
};