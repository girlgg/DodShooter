﻿#include "Character/DodCharacter.h"

#include "DodGameplayTags.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/DodAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/DodCameraComponent.h"
#include "Character/DodCharacterMovementComp.h"
#include "Character/Comp/DodHealthComponent.h"
#include "Character/Comp/DodPawnExtensionComponent.h"
#include "Character/Comp/DodStaminaComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Team/DodTeamAgentInterface.h"

ADodCharacter::ADodCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDodCharacterMovementComp>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -90.f));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 0.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UDodCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Head");

	PawnExtComponent = CreateDefaultSubobject<UDodPawnExtensionComponent>(TEXT("PawnExt"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<UDodHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	StaminaComponent = CreateDefaultSubobject<UDodStaminaComponent>(TEXT("StaminaComponent"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
}

UDodAbilitySystemComponent* ADodCharacter::GetDodAbilitySystemComponent() const
{
	return Cast<UDodAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ADodCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetDodAbilitySystemComponent();
}

void ADodCharacter::ToggleCrouch()
{
	const UDodCharacterMovementComp* DodMoveComp = CastChecked<UDodCharacterMovementComp>(GetCharacterMovement());

	if (bIsCrouched || DodMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (DodMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void ADodCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADodCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID)
}

void ADodCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UDodAbilitySystemComponent* ASC = GetDodAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(DodGameplayTags::Status_Crouching, 1);
	}
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ADodCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UDodAbilitySystemComponent* ASC = GetDodAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(DodGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ADodCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
	}
}

FGenericTeamId ADodCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnDodTeamIndexChangedDelegate* ADodCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ADodCharacter::AddControllerRotation(FRotator InRotator)
{
	AddControllerPitchInput(InRotator.Pitch);
	AddControllerRollInput(InRotator.Roll);
	AddControllerYawInput(InRotator.Yaw);
}

void ADodCharacter::OnAbilitySystemInitialized()
{
	UDodAbilitySystemComponent* ASC = GetDodAbilitySystemComponent();
	check(ASC);

	HealthComponent->InitializeWithAbilitySystem(ASC);
	StaminaComponent->InitializeWithAbilitySystem(ASC);

	InitializeGameplayTags();
}

void ADodCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
	StaminaComponent->UninitializeFromAbilitySystem();
}

void ADodCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	if (IDodTeamAgentInterface* ControllerAsTeamProvider = Cast<IDodTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ADodCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void ADodCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ADodCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void ADodCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnExtComponent->SetupPlayerInputComponent();
}

void ADodCharacter::InitializeGameplayTags()
{
	if (UDodAbilitySystemComponent* ASC = GetDodAbilitySystemComponent())
	{
	}
}

void ADodCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void ADodCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ADodCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ADodCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void ADodCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ADodCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	if (UDodAbilitySystemComponent* ASC = GetDodAbilitySystemComponent())
	{
		if (ASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ADodCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void ADodCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}
