#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_WallRunnable ECC_GameTraceChannel3

const FName SocketFPCamera = FName("CameraSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryWallrun = FName("Wallrun");
const FName DebugCategorySlide= FName("Slide");

UENUM(BlueprintType)
enum class EWallrunSide : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};