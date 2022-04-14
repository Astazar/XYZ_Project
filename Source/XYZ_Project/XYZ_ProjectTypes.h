#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_WallRunnable ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
#define ECC_Melee ECC_GameTraceChannel5

const FName FXParamTraceEnd = FName("TraceEnd");

const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");
const FName SocketCharacterThrowable = FName("ThrowableSocket");

const FName SectionMontageReloadEnd = FName("ReloadEnd");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfileNoCollision = FName("NoCollision");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryWallrun = FName("Wallrun");
const FName DebugCategorySlide= FName("Slide");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName DebugCategoryMeleeWeapon = FName("MeleeWeapon");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_NextLocation = FName("NextLocation");

UENUM(BlueprintType)
enum class EWallrunSide : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle,
	Throwable,
	Melee
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	ShotgunShells,
	FragGrenades,
	RifleGrenades,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class  EEquipmentSlots : uint8
{
	None,
	SideArm, 
	PrimaryWeapon,
	SecondaryWeapon,
	PrimaryItemSlot,
	MeleeWeapon,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReticleType : uint8
{
	None,
	Default,
	SniperRifle,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMeleeAttackTypes : uint8
{
	None,
	PrimaryAttack,
	SecondaryAttack,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	Player,
	Enemy
};