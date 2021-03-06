// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/TimelineComponent.h>
#include <Curves/CurveVector.h>
#include "XYZ_ProjectTypes.h"
#include <GenericTeamAgentInterface.h>
#include <UObject/ScriptInterface.h>
#include "XYZBaseCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmimingStateChanged, bool);
DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName)

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SettingsName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimMaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimMinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSettingApplyHeight = 250.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};


class UXYZBaseMovementComponent;
class AInteractiveActor;
class UCharacterEquipmentComponent;
class IInteractable;
class UWidgetComponent;
typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray; 

UCLASS(Abstract, NotBlueprintable) 
class XYZ_PROJECT_API AXYZBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AXYZBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;

	void StartFire();
	void StopFire();

	void Reload();

	FOnAmimingStateChanged OnAmimingStateChanged;
	FRotator GetAimOffsetLocal();
	void StartAiming();
	void StopAiming();
	bool IsAiming() const;
	float GetAimingMovementSpeed() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();
	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	void NextItem();
	void PreviousItem();

	void NextWeaponBarell();

	void ThrowPrimaryItem(bool bShouldEquip);
	void OnStartThrow();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void Interact();
	FOnInteractableObjectFound OnInteractableObjectFound;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual bool CanCrouch() const override;

	virtual void ChangeCrouchState();
	virtual void ChangeCrawlState();

	virtual bool CanCrawl() const;
	virtual void Crawl();
	virtual void Uncrawl();

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void StartSprint();
	virtual void StopSprint();

	virtual bool CanSlide();
	virtual	void Slide();
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};
	virtual void SwimDive() {};

	virtual void OnMantle(const FMantlingSettings* MantlingSettings, float MantlingAnimationStartTime);
	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);
	virtual bool CanMantle() const;
	UPROPERTY(ReplicatedUsing=OnRep_IsMantling)
	bool bIsMantling = false;
	UFUNCTION()
	void OnRep_IsMantling(bool bWasMantling);

	UFUNCTION(BlueprintCallable)
	virtual UXYZBaseMovementComponent* GetCharacterMovementComponent() const;

	UFUNCTION(BlueprintCallable)
	class UCharacterAttributesComponent* GetCharacterAttributesComponent() const { return CharacterAttributesComponent; }

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	float GetIKRightFootOffset() const { return IKRightFootOffset; }
	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }
	float GetIKPelvisOffset() const { return IKPelvisOffset; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	bool CanInteractWithLadder();
	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	const class ALadder* GetAvailableLadder() const; 

	virtual void InteractWithZipline();
	virtual void ZiplineClimbForward(float Value);
	virtual void ZiplineTurnAround();
	const class AZipline* GetAvailableZipline() const;

	virtual void Wallrun();
	virtual bool CanWallrun();

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
	virtual void HardLanded();

	virtual void LimitControl();
	virtual void UnlimitControl();

	void SelectMovementSettings(bool bEnableFreeLook);

	void AddEquipmentItem(const TSubclassOf<class AEquipableItem> EquipableItemClass);

/* IGenericTeamAgentInterface */
	virtual FGenericTeamId GetGenericTeamId() const override;
/* ~IGenericTeamAgentInterface */

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UXYZBaseMovementComponent* XYZBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterAttributesComponent* CharacterAttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character | Components")
	UWidgetComponent* HealthBarProgressComponent;

	void InitializeHealthProgress();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	virtual bool CanSprint();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	float IKInterpSpeed = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | IK settings")
	float UnderFeetTraceLenght = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	TArray<FMantlingSettings> MantlingSettingsCollection;

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* HardLandingAnimMontage;
	//The height from which the hard landing montage will play (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	float HardLandingHeight = 8.0f;

	//Damage depending from fall height (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes | Damage")
	class UCurveFloat* FallDamageCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes | Damage", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfOxygenDamageInterval = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes | Damage", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfOxygenDamage = 20.0f;

	void OutOfOxygenTakeDamage();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacer | Team")
	ETeams Team = ETeams::Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Chatacer | Interaction")
	float LineOfSightDistance = 500.0f;

	void TraceLineOfSight();

	UPROPERTY()
	TScriptInterface<IInteractable> LineOfSightObject;

private:
	FVector CurrentFallApex;

	void EnableRagdoll();

	void UpdateIKOffsets(float DeltaSeconds);
	void TryChangeSprintState(float DeltaSeconds);

	virtual void UpdateOutOfOxygenDamage(float DeltaSeconds);

	bool bIsSprintRequested = false;

	bool bIsAiming = false;
	float CurrentAimingMovementSpeed = 0.0f;

	float GetIKOffsetForASocket(const FName& SocketName);
	float CalculateIKPelvisOffset();

	const FMantlingSettings* GetMantlingSettings(float LedgeHeight) const;

	TInteractiveActorsArray AvailableInteractiveActors;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKPelvisOffset = 0.0f;
	float IKTraceDistance = 0.0f;
	float IKScale = 0.0f;

	FTimerHandle OutOfOxygenDamageTimer;
	FTimerHandle HardLandingTimer;
};
