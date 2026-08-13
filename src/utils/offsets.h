#pragma once
#include <cstdint>

namespace Offsets {
  namespace IL2CPP {
    // These are resolved dynamically via GetProcAddress
    // Listed here for reference only
  }  // namespace IL2CPP

  // ==========================================
  // WeaponManager (TypeDefIndex: 5805)
  // Static singleton — main weapon system controller
  // ==========================================
  namespace WeaponManager {
    // Static fields (offsets into static field data)
    constexpr uintptr_t StaticInstance = 0x230;  // WeaponManager singleton

    // Instance fields
    constexpr uintptr_t myPlayer            = 0x40;  // GameObject*
    constexpr uintptr_t myPlayerMoveC       = 0x48;  // Player_move_c*
    constexpr uintptr_t myGun               = 0x50;  // GameObject*
    constexpr uintptr_t myNetworkStartTable = 0x60;  // NetworkStartTable*
    constexpr uintptr_t currentWeaponSounds = 0x98;  // WeaponSounds* (currently held weapon)
  }  // namespace WeaponManager

  // ==========================================
  // Player_move_c (TypeDefIndex: 1241)
  // Main player controller — enormous class (~0x1090+ bytes)
  // ==========================================
  namespace PlayerMoveC {
    // Weapon system
    constexpr uintptr_t useNewWeaponSystem      = 0x30;   // bool
    constexpr uintptr_t charWeapon              = 0xBE0;  // weapon struct array
    constexpr uintptr_t charWeaponCurrent       = 0xBE8;  // current weapon struct
    constexpr uintptr_t extractionWeaponCurrent = 0xBF8;  // extraction mode weapon

    // Colliders (for aimbot targeting)
    constexpr uintptr_t headCollider     = 0x158;  // BoxCollider*
    constexpr uintptr_t _aimCollider     = 0x180;  // Collider*
    constexpr uintptr_t _bodyAimCollider = 0x188;  // Collider*
    constexpr uintptr_t _headAimCollider = 0x190;  // Collider*

    // Transforms
    constexpr uintptr_t myPlayerTransform   = 0x3F8;  // Transform*
    constexpr uintptr_t myWeaponTransform   = 0x400;  // Transform*
    constexpr uintptr_t PlayerHeadTransform = 0x4B0;  // Transform*
    constexpr uintptr_t myCameraContainer   = 0x3E8;  // Transform*

    // Networking
    constexpr uintptr_t photonView       = 0x440;  // PhotonView*
    constexpr uintptr_t photonViewGroup0 = 0x448;  // PhotonView*
    constexpr uintptr_t pixelView        = 0x450;  // PixelView*
    constexpr uintptr_t myPersonNetwork  = 0x3F0;  // PlayerSynchStream*

    // References
    constexpr uintptr_t weaponManagerRef  = 0x7F0;  // WeaponManager*
    constexpr uintptr_t weaponSoundsRef   = 0x6C0;  // WeaponSounds* (current)
    constexpr uintptr_t playerDamageable  = 0x6E8;  // PlayerDamageable*
    constexpr uintptr_t myScoreController = 0x408;  // PlayerScoreController*
    constexpr uintptr_t consumables       = 0x348;  // PlayerConsumables*
    constexpr uintptr_t visibleObjRef     = 0x7C8;  // visibleObjPhoton*

    // Visual / UI
    constexpr uintptr_t nickLabel                = 0x418;  // TextMesh*
    constexpr uintptr_t nickLabelContainer       = 0x410;  // GameObject*
    constexpr uintptr_t nickLabelDefaultMaterial = 0xD48;  // Material*
    constexpr uintptr_t nickLabelThroughWallMat  = 0xD50;  // Material*
    constexpr uintptr_t gunCamera                = 0x2D8;  // Camera*
    constexpr uintptr_t armorPoint               = 0x1A8;  // GameObject*
    constexpr uintptr_t enemyBacklight           = 0x1B8;  // CameraFacingBilloard*
    constexpr uintptr_t allyBacklight            = 0x1C0;  // CameraFacingBilloard*

    // Renderers (for ESP bone access)
    constexpr uintptr_t playerBodyRenderer = 0x488;  // SkinnedMeshRenderer*
    constexpr uintptr_t playerHeadRenderer = 0x490;  // SkinnedMeshRenderer*
    constexpr uintptr_t playerArmsRenderer = 0x498;  // SkinnedMeshRenderer*
    constexpr uintptr_t playerLegsRenderer = 0x4A0;  // SkinnedMeshRenderer*

    // Health bar
    constexpr uintptr_t HealthBarPlayerBody    = 0x1038;  // HealthBarByMeshRenderer*
    constexpr uintptr_t HasHealthBarPlayerBody = 0x1040;  // bool
    constexpr uintptr_t HealthBarMechBody      = 0x1048;  // HealthBarByMeshRenderer*
    constexpr uintptr_t HasHealthBarMechBody   = 0x1050;  // bool

    // State flags
    constexpr uintptr_t isMoving               = 0xCE4;   // bool
    constexpr uintptr_t IsWeaponDelay          = 0xCEF;   // bool
    constexpr uintptr_t WeaponDelayTimeForFire = 0xCF0;   // float
    constexpr uintptr_t lastDamageTime         = 0x108C;  // float
    constexpr uintptr_t delayForHideChargeBar  = 0x628;   // float
    constexpr uintptr_t healingByPlayer        = 0x1080;  // Player_move_c*
    constexpr uintptr_t gadgetReflectorCoeff   = 0xDA0;   // float

    // Weapon kills tracking
    constexpr uintptr_t weaponsKills = 0xBC0;  // Dictionary<int,int>*

    // FPS Player body
    constexpr uintptr_t fpsPlayerBody = 0x2C8;  // GameObject*

    // Shoulder zoom
    constexpr uintptr_t shoulderZoomFovForScope = 0x4D0;  // float
  }  // namespace PlayerMoveC

  // ==========================================
  // WeaponSounds (TypeDefIndex: 5877)
  // Per-weapon configuration — the main target for weapon mods
  // ==========================================
  namespace WeaponSounds {
    // --- Ammo ---
    constexpr uintptr_t categoryNabor   = 0x58;   // int (weapon category slot)
    constexpr uintptr_t ammoInClip      = 0x6C;   // int
    constexpr uintptr_t InitialAmmo     = 0x70;   // int
    constexpr uintptr_t maxAmmo         = 0x74;   // int
    constexpr uintptr_t isUnlimitedAmmo = 0x54A;  // bool

    // --- Scatter (Spread) ---
    constexpr uintptr_t tekKoof          = 0x9C;  // float (base scatter)
    constexpr uintptr_t upKoofFire       = 0xA0;  // float (scatter increase on fire)
    constexpr uintptr_t downKoofFirst    = 0xA4;  // float (first shot recovery)
    constexpr uintptr_t downKoof         = 0xA8;  // float (scatter recovery)
    constexpr uintptr_t moveScatterCoeff = 0xC4;  // float (movement scatter)
    constexpr uintptr_t firstShotScatter = 0xBC;  // bool
    constexpr uintptr_t scatterInversion = 0xCA;  // bool

    // --- Scatter (Zoom) ---
    constexpr uintptr_t startZoneZoom        = 0x108;  // Vector2
    constexpr uintptr_t maxKoofZoom          = 0x110;  // float
    constexpr uintptr_t upKoofFireZoom       = 0x114;  // float
    constexpr uintptr_t downKoofFirstZoom    = 0x118;  // float
    constexpr uintptr_t downKoofZoom         = 0x11C;  // float
    constexpr uintptr_t recoilCoeffZoom      = 0x130;  // float
    constexpr uintptr_t firstShotScatterZoom = 0x124;  // bool
    constexpr uintptr_t moveScatterCoeffZoom = 0x128;  // float

    // --- Zoom ---
    constexpr uintptr_t isZooming       = 0xCC;  // bool
    constexpr uintptr_t zoomXray        = 0xCD;  // bool
    constexpr uintptr_t fieldOfViewZoom = 0xE0;  // float

    // --- Charge ---
    constexpr uintptr_t isCharging = 0x1FA;  // bool
    constexpr uintptr_t chargeTime = 0x214;  // float
    constexpr uintptr_t chargeMax  = 0x210;  // int
    constexpr uintptr_t chargeLoop = 0x208;  // bool

    // --- Melee ---
    constexpr uintptr_t isMelee                 = 0x191;  // bool
    constexpr uintptr_t radiusRoundMelee        = 0x1A0;  // float
    constexpr uintptr_t meleeAttackTimeModifier = 0x1A8;  // float
    constexpr uintptr_t isPiercingMelee         = 0x1AE;  // bool
    constexpr uintptr_t distancePiercingMelee   = 0x1B0;  // float
    constexpr uintptr_t meleePunchesCount       = 0x1B8;  // int
    constexpr uintptr_t IsBackstab              = 0x1BC;  // bool
    constexpr uintptr_t BackstabMultiplier      = 0x1C0;  // float

    // --- Bullet ---
    constexpr uintptr_t countShots  = 0x1C4;  // int (pellets per shot)
    constexpr uintptr_t bulletDelay = 0x1D4;  // float
    constexpr uintptr_t shootDelay  = 0x1D8;  // float
    constexpr uintptr_t isShotGun   = 0x1CE;  // bool

    // --- Bazooka / Explosive ---
    constexpr uintptr_t bazooka                 = 0x140;  // bool
    constexpr uintptr_t bazookaDelay            = 0x144;  // float
    constexpr uintptr_t bazookaExplosionRadius  = 0x158;  // float
    constexpr uintptr_t bazookaExplosionRadSelf = 0x15C;  // float
    constexpr uintptr_t impulseForce            = 0x174;  // float
    constexpr uintptr_t impulseForceSelf        = 0x178;  // float
    constexpr uintptr_t rocketNum               = 0x148;  // int

    // --- Critical ---
    constexpr uintptr_t criticalHitChance = 0x564;  // int (0-100)
    constexpr uintptr_t criticalHitCoef   = 0x568;  // float (multiplier)

    // --- Speed ---
    constexpr uintptr_t speedModifier = 0x774;  // float

    // --- Delay ---
    constexpr uintptr_t DelayTimer = 0x550;  // float

    // --- Burst ---
    constexpr uintptr_t isBurstShooting   = 0x557;  // bool
    constexpr uintptr_t countShootInBurst = 0x6DC;  // int

    // --- Loop ---
    constexpr uintptr_t isLoopShoot = 0x560;  // bool
    constexpr uintptr_t seriesShoot = 0x556;  // bool

    // --- AOE / Sectors ---
    constexpr uintptr_t isSectorsAOE           = 0x484;  // bool
    constexpr uintptr_t sectorsAOEAngleFront   = 0x488;  // float
    constexpr uintptr_t sectorsAOEAngleBack    = 0x48C;  // float
    constexpr uintptr_t sectorsAOEDmgMultFront = 0x490;  // float
    constexpr uintptr_t sectorsAOEDmgMultSide  = 0x494;  // float
    constexpr uintptr_t sectorsAOEDmgMultBack  = 0x498;  // float
    constexpr uintptr_t sectorsAOERadius       = 0x49C;  // float

    // --- Damage ---
    constexpr uintptr_t damageByTier = 0x6E8;  // float[]
    constexpr uintptr_t damageRange  = 0x6F0;  // Vector2
    constexpr uintptr_t fireRateShop = 0x718;  // int

    // --- Effects ---
    constexpr uintptr_t isDamageHeal           = 0x228;  // bool
    constexpr uintptr_t damageHealMultiplier   = 0x22C;  // float
    constexpr uintptr_t isPoisoning            = 0x238;  // bool
    constexpr uintptr_t poisonDamageMultiplier = 0x240;  // float
    constexpr uintptr_t isSlowdown             = 0x270;  // bool
    constexpr uintptr_t slowdownCoeff          = 0x274;  // float
    constexpr uintptr_t isStun                 = 0x284;  // bool
    constexpr uintptr_t stunCoeff              = 0x288;  // float

    // --- Invisibility ---
    constexpr uintptr_t isInvisibleReload      = 0x444;  // bool
    constexpr uintptr_t isInvisibleAfterKill   = 0x445;  // bool
    constexpr uintptr_t invisibleAfterKillTime = 0x448;  // float

    // --- Auto targeting ---
    constexpr uintptr_t IsActiveAim = 0x5C0;  // bool

    // --- Misc ---
    constexpr uintptr_t weaponIsNotFromConfig = 0x591;  // bool
    constexpr uintptr_t is3CatSpam            = 0x7A8;  // bool
  }  // namespace WeaponSounds

  // ==========================================
  // PlayerDamageable (TypeDefIndex: 1711)
  // Damage handler — for god mode hook
  // ==========================================
  namespace PlayerDamageable {
    constexpr uintptr_t playerMoveC = 0x18;  // Player_move_c*

    // Method RVAs (relative to GameAssembly.dll base)
    constexpr uintptr_t ApplyDamage_RVA   = 0x1DB8410;
    constexpr uintptr_t IsDead_RVA        = 0x1DB9940;
    constexpr uintptr_t IsEnemyTo_RVA     = 0x1DB9960;
    constexpr uintptr_t get_maxHealth_RVA = 0x1DBA350;
  }  // namespace PlayerDamageable

  // ==========================================
  // UnityEngine.Object (TypeDefIndex: 12345)
  // ==========================================
  namespace Object {
    constexpr uintptr_t FindObjectsOfType_RVA = 0x476DFF0;
  }

  // ==========================================
  // FirstPersonControlSharp (TypeDefIndex: 6617)
  // FPS movement and camera controller
  // ==========================================
  namespace FPSController {
    constexpr uintptr_t _moveC                  = 0xC0;   // Player_move_c*
    constexpr uintptr_t characterController     = 0xE8;   // CharacterController*
    constexpr uintptr_t speedCoefInjured        = 0x194;  // float
    constexpr uintptr_t accuracyMoveModifier    = 0x50;   // float
    constexpr uintptr_t camTweenerFallDamage    = 0x428;  // TweenRotation*
    constexpr uintptr_t maxNegVelocityYPerFrame = 0x4F4;  // float
    constexpr uintptr_t maxPosVelocityYPerFrame = 0x4F8;  // float
    constexpr uintptr_t velocityDownFallMult    = 0x4E8;  // float
    constexpr uintptr_t playerCamera            = 0x1A8;  // BaseCamera*
  }  // namespace FPSController

  // ==========================================
  // Anti-Cheat Systems
  // ==========================================
  namespace AntiCheat {
    // CheatDetectedBanner (TypeDefIndex: 352) — OBSOLETE but still present
    constexpr uintptr_t CBD_Trigger_RVA    = 0x218B940;  // static trigger method
    constexpr uintptr_t CBD_Awake_RVA      = 0x218B570;
    constexpr uintptr_t CBD_Update_RVA     = 0x218BB20;
    constexpr uintptr_t CBD_ShowBanner_RVA = 0x218B9C0;  // static show method

    // ClickerDetector (TypeDefIndex: 421) — monitors input patterns
    // Need to find its Update/check methods dynamically
  }  // namespace AntiCheat

  // ==========================================
  // InnerWeaponPars (TypeDefIndex: 4024)
  // Internal weapon parameters
  // ==========================================
  namespace InnerWeaponPars {
    constexpr uintptr_t charge       = 0xF8;   // AudioClip*
    constexpr uintptr_t chargedShoot = 0x118;  // AudioClip*
  }  // namespace InnerWeaponPars

  // ==========================================
  // ExtractionWeaponSettings (TypeDefIndex: 3385)
  // ==========================================
  namespace ExtractionWeaponSettings {
    constexpr uintptr_t ammoInClip = 0x20;  // int
    constexpr uintptr_t ammo       = 0x24;  // int
  }  // namespace ExtractionWeaponSettings

  // ==========================================
  // IL2CPP Class pointers (resolved at runtime, cached here)
  // These will be populated by il2cpp_resolve_classes()
  // ==========================================
  namespace Classes {
    inline uintptr_t WeaponManager       = 0;
    inline uintptr_t PlayerMoveC         = 0;
    inline uintptr_t WeaponSounds        = 0;
    inline uintptr_t PlayerDamageable    = 0;
    inline uintptr_t FPSController       = 0;
    inline uintptr_t CheatDetectedBanner = 0;
    inline uintptr_t ClickerDetector     = 0;
  }  // namespace Classes
}  // namespace Offsets
