// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "XYZ_ProjectBasePawn.generated.h"

UCLASS()
class XYZ_PROJECT_API AXYZ_ProjectBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AXYZ_ProjectBasePawn();

	UPROPERTY(VisibleAnywhere)
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveRight(float value);

	void MoveForward(float value);

	void Jump();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputForward() {return InputForward;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetInputRight() { return InputRight; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCollisionSphereRadius() { return CollisionSphereRadius; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base pawn")
	float CollisionSphereRadius = 50.0f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Base pawn")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UCameraComponent* CameraComponent;

#if	WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	class UArrowComponent* ArrowComponent;
#endif



private:
	UFUNCTION()
	void OnBlendComplete();

	AActor* CurrentViewActor;

	float InputForward;
	float InputRight;
};
