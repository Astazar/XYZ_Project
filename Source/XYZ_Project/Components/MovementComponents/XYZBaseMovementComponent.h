// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XYZBaseMovementComponent.generated.h"



UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Standing = 0,
	Crawling
};


UCLASS()
class XYZ_PROJECT_API UXYZBaseMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	bool IsSprinting() { return bIsSprinting; }
	bool IsCrawling() { return bIsCrawling; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	virtual void Crawl();
	virtual void Uncrawl();

	virtual bool IsEnoughSpaceToUncrouch();
	virtual bool IsEnoughSpaceToUncrawl();

	bool GetIsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	bool bWantsToCrawl = false;

	EMovementState PreviousMovementState = EMovementState::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlingHalfHeight = 34.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Crawl", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float CrawlSpeed = 100.0f;

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsCrawling = false;

};
