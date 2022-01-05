#pragma once

#include "CoreMinimal.h"
#include "XYZ_Project/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"


UENUM(BlueprintType)
enum class EZiplineMovementType : uint8
{
	Slide = 0,
	Climb
};

class UStaticMeshComponent;
class UBoxComponent;

UCLASS(Blueprintable)
class XYZ_PROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
	
public:
	AZipline();

	virtual void OnConstruction(const FTransform& Transform) override;

	float GetCableLenght() const;

	FVector GetStartPillarTopWorldLocation() const;

	FVector GetEndPillarTopWorldLocation() const;

	FVector GetCableMeshLocation() const;

	const UStaticMeshComponent* GetCableMesh() const;

	EZiplineMovementType GetZiplineMovementType() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* StartPillarMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* EndPillarMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* CableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting")
	EZiplineMovementType ZiplineMovementType = EZiplineMovementType::Slide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting")
	float StartPillarHeight = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting")
	float EndPillarHeight = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting")
	float InteractionCapsuleRadius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting", meta = (MakeEditWidget))
	FVector StartPillarLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline setting", meta = (MakeEditWidget))
	FVector EndPillarLocation = FVector(0, 100, 0); //Adding some initial offset to end pillar to avoid stuck

	virtual void OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	FVector StartPillarTop = FVector::ZeroVector;
	FVector EndPillarTop = FVector::ZeroVector;

	float CableLenght = 0.0f;
};
