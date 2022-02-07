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
}

void ARangeWeaponItem::Fire()
{
	checkf(GetOwner()->IsA<AXYZBaseCharacter>(), TEXT("ARangeWeaponItem::Fire() can work only with AXYZBaseCharacter"));
	AXYZBaseCharacter* CharacterOwner = StaticCast<AXYZBaseCharacter*>(GetOwner());
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	
	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FVector PlayerViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	WeaponBarell->Shot(PlayerViewPoint, PlayerViewDirection, Controller);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	return WeaponAnimInstance->Montage_Play(AnimMontage);
}
