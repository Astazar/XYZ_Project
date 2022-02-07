// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XYZ_Project/XYZ_ProjectTypes.h"
#include "XYZBaseCharacterAnimInstance.generated.h"


/**
 * 
 */
UCLASS()
class XYZ_PROJECT_API UXYZBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsFalling = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsCrouching = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOutOfStamina = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsCrawling = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsSwimming = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsMantling = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsOnLadder = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsZiplining = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsWallrunning = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = -180, UIMax = 180))
	float Direction = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation", meta = (UIMin = 0, UIMax = 500))
	float Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float Stamina = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	float LadderSpeedRatio = 0.0f;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK Settings")
	FVector RightFootEffectorLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK Settings")
	FVector LeftFootEffectorLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK Settings")
	FVector PelvisOffsetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EWallrunSide CurrentWallrunSide = EWallrunSide::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	EEquipableItemType CurrentEquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
	FRotator AimRotation = FRotator::ZeroRotator;

private:
	TWeakObjectPtr<class AXYZBaseCharacter> CachedBaseCharacter;
};
