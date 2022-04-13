// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AI/Perception/Senses/AISense_DamageSight.h"
#include "AISenseConfig_DamageSight.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "AI DamageSight sense config"))
class XYZ_PROJECT_API UAISenseConfig_DamageSight : public UAISenseConfig
{
	GENERATED_BODY()
	
public:
	UAISenseConfig_DamageSight(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
	TSubclassOf<UAISense_DamageSight> Implementation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float SightRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0f, ClampMin = 0.0f, UIMax = 180.0f, ClampMax = 180.0f, DisplayName = "SightHalfAngleDegrees"))
	float SightAngle = 30.0f;

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;
};
