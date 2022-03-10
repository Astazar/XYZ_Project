// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"
#include <Components/Weapon/WeaponBarellComponent.h>
#include "XYZ_ProjectTypes.h"
#include <Characters/XYZBaseCharacter.h>


ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);
	CurrentWeaponBarell = WeaponBarell;
	CurrentBarellIndex = 0;

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents(BarellsArray);
}

void ARangeWeaponItem::StartFire()
{	
	if (GetWorldTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}

	bIsFiring = true;
	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

void ARangeWeaponItem::StartReload()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	bIsReloading = true;
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if (ReloadType == EReloadType::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
		}
	}
	else
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsReloading)
	{
		return;
	}

	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	if (!bIsSuccess)
	{
		CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		StopAnimMontage(WeaponReloadMontage);
	}

	if (ReloadType == EReloadType::ByBullet)
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);

	bIsReloading = false;
	if (OnReloadComplete.IsBound() && bIsSuccess)
	{
		OnReloadComplete.Broadcast();
	}
}

void ARangeWeaponItem::NextWeaponBarell()
{
	if (CurrentBarellIndex != BarellsArray.Num() - 1)
	{
		CurrentBarellIndex++;
	}
	else
	{
		CurrentBarellIndex = 0;
	}
	CurrentWeaponBarell = BarellsArray[CurrentBarellIndex];
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	return AimTurnModifier;
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

UWeaponBarellComponent* ARangeWeaponItem::GetCurrentBarellComponent() const
{
	return CurrentWeaponBarell;
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming ? AimReticleType : ReticleType;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}


void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}
		case EWeaponFireMode::FullAuto:
		{
			MakeShot();
			break;
		}
	}
}

void ARangeWeaponItem::MakeShot()
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(CharacterOwner) || !IsValid(Controller))
	{
		return;
	}

	if (!CurrentWeaponBarell->CanShoot())
	{
		StopFire();
		if (CurrentWeaponBarell->GetAmmo() == 0 && bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}

	EndReload(false);

	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	CurrentWeaponBarell->Shot(PlayerViewPoint, PlayerViewDirection, GetCurrentBulletSpreadAngle());
	CurrentWeaponBarell->SetAmmo(CurrentWeaponBarell->GetAmmo()-1);

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegrees);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / RateOfFire;
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime /*= 0.0f*/)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
	}
}
