#include "MeleeWeaponItem.h"
#include "Characters/XYZBaseCharacter.h"

AMeleeWeaponItem::AMeleeWeaponItem()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackTypes AttackType)
{
	AXYZBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack != nullptr && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackTimerElapsed, Duration, false);
		}
		else
		{
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeaponItem::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
}
