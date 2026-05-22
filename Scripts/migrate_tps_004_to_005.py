"""
Migrate Blueprint parent classes and soft references from TPS_004_git to TPS_005_git.
Run in UE: py "../../../../Unreal Projects/TPS_005_git/Scripts/migrate_tps_004_to_005.py"
"""
import unreal

OLD_MODULE = "TPS_004_git"
NEW_MODULE = "TPS_005_git"
OLD_SCRIPT = f"/Script/{OLD_MODULE}"
NEW_SCRIPT = f"/Script/{NEW_MODULE}"

# Blueprints whose parent C++ class lives in the game module (name may include TPS_004_git prefix).
REPARENT_MAP = {
    f"{OLD_SCRIPT}.TPS_004_gitCharacter": f"{NEW_SCRIPT}.TPS_005_gitCharacter",
    f"{OLD_SCRIPT}.TPS_004_gitPlayerController": f"{NEW_SCRIPT}.TPS_005_gitPlayerController",
    f"{OLD_SCRIPT}.TPS_004_gitGameMode": f"{NEW_SCRIPT}.TPS_005_gitGameMode",
    f"{OLD_SCRIPT}.CombatCharacter": f"{NEW_SCRIPT}.CombatCharacter",
    f"{OLD_SCRIPT}.CombatPlayerController": f"{NEW_SCRIPT}.CombatPlayerController",
    f"{OLD_SCRIPT}.CombatGameMode": f"{NEW_SCRIPT}.CombatGameMode",
    f"{OLD_SCRIPT}.CombatEnemy": f"{NEW_SCRIPT}.CombatEnemy",
    f"{OLD_SCRIPT}.CombatAIController": f"{NEW_SCRIPT}.CombatAIController",
    f"{OLD_SCRIPT}.CombatEnemySpawner": f"{NEW_SCRIPT}.CombatEnemySpawner",
    f"{OLD_SCRIPT}.CombatActivationVolume": f"{NEW_SCRIPT}.CombatActivationVolume",
    f"{OLD_SCRIPT}.CombatCheckpointVolume": f"{NEW_SCRIPT}.CombatCheckpointVolume",
    f"{OLD_SCRIPT}.CombatDamageableBox": f"{NEW_SCRIPT}.CombatDamageableBox",
    f"{OLD_SCRIPT}.CombatLavaFloor": f"{NEW_SCRIPT}.CombatLavaFloor",
    f"{OLD_SCRIPT}.CombatDummy": f"{NEW_SCRIPT}.CombatDummy",
    f"{OLD_SCRIPT}.PlatformingCharacter": f"{NEW_SCRIPT}.PlatformingCharacter",
    f"{OLD_SCRIPT}.PlatformingPlayerController": f"{NEW_SCRIPT}.PlatformingPlayerController",
    f"{OLD_SCRIPT}.PlatformingGameMode": f"{NEW_SCRIPT}.PlatformingGameMode",
    f"{OLD_SCRIPT}.SideScrollingCharacter": f"{NEW_SCRIPT}.SideScrollingCharacter",
    f"{OLD_SCRIPT}.SideScrollingPlayerController": f"{NEW_SCRIPT}.SideScrollingPlayerController",
    f"{OLD_SCRIPT}.SideScrollingGameMode": f"{NEW_SCRIPT}.SideScrollingGameMode",
    f"{OLD_SCRIPT}.SideScrollingCameraManager": f"{NEW_SCRIPT}.SideScrollingCameraManager",
    f"{OLD_SCRIPT}.SideScrollingNPC": f"{NEW_SCRIPT}.SideScrollingNPC",
    f"{OLD_SCRIPT}.SideScrollingAIController": f"{NEW_SCRIPT}.SideScrollingAIController",
    f"{OLD_SCRIPT}.SideScrollingMovingPlatform": f"{NEW_SCRIPT}.SideScrollingMovingPlatform",
    f"{OLD_SCRIPT}.SideScrollingJumpPad": f"{NEW_SCRIPT}.SideScrollingJumpPad",
    f"{OLD_SCRIPT}.SideScrollingPickup": f"{NEW_SCRIPT}.SideScrollingPickup",
    f"{OLD_SCRIPT}.SideScrollingSoftPlatform": f"{NEW_SCRIPT}.SideScrollingSoftPlatform",
}


def _load_class(class_path: str):
    return unreal.load_class(None, class_path)


# Explicit list of game blueprints and their TPS_005_git C++ parents.
BLUEPRINT_FIXES = [
    ("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter", "TPS_005_gitCharacter"),
    ("/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController", "TPS_005_gitPlayerController"),
    ("/Game/ThirdPerson/Blueprints/BP_ThirdPersonGameMode", "TPS_005_gitGameMode"),
    ("/Game/Variant_Combat/Blueprints/BP_CombatCharacter", "CombatCharacter"),
    ("/Game/Variant_Combat/Blueprints/BP_CombatPlayerController", "CombatPlayerController"),
    ("/Game/Variant_Combat/Blueprints/BP_CombatGameMode", "CombatGameMode"),
    ("/Game/Variant_Combat/Blueprints/AI/BP_CombatEnemy", "CombatEnemy"),
    ("/Game/Variant_Combat/Blueprints/AI/BP_CombatAIController", "CombatAIController"),
    ("/Game/Variant_Combat/Blueprints/AI/BP_CombatEnemySpawner", "CombatEnemySpawner"),
    ("/Game/Variant_Combat/Blueprints/Interactables/BP_CombatActivationVolume", "CombatActivationVolume"),
    ("/Game/Variant_Combat/Blueprints/Interactables/BP_CombatCheckpointVolume", "CombatCheckpointVolume"),
    ("/Game/Variant_Combat/Blueprints/Interactables/BP_CombatDamageableBox", "CombatDamageableBox"),
    ("/Game/Variant_Combat/Blueprints/Interactables/BP_CombatDummy", "CombatDummy"),
    ("/Game/Variant_Combat/Blueprints/Interactables/BP_CombatLavaFloor", "CombatLavaFloor"),
    ("/Game/Variant_Platforming/Blueprints/BP_PlatformingCharacter", "PlatformingCharacter"),
    ("/Game/Variant_Platforming/Blueprints/BP_PlatformingPlayerController", "PlatformingPlayerController"),
    ("/Game/Variant_Platforming/Blueprints/BP_PlatformingGameMode", "PlatformingGameMode"),
    ("/Game/Variant_SideScrolling/Blueprints/BP_SideScrollingCharacter", "SideScrollingCharacter"),
    ("/Game/Variant_SideScrolling/Blueprints/BP_SideScrollingPlayerController", "SideScrollingPlayerController"),
    ("/Game/Variant_SideScrolling/Blueprints/BP_SideScrollingGameMode", "SideScrollingGameMode"),
    ("/Game/Variant_SideScrolling/Blueprints/BP_SideScrollingCameraManager", "SideScrollingCameraManager"),
    ("/Game/Variant_SideScrolling/Blueprints/AI/BP_SideScrollingNPC", "SideScrollingNPC"),
    ("/Game/Variant_SideScrolling/Blueprints/AI/BP_SideScrollingAIController", "SideScrollingAIController"),
    ("/Game/Variant_SideScrolling/Blueprints/Items/BP_SideScrollingMovingPlatform", "SideScrollingMovingPlatform"),
    ("/Game/Variant_SideScrolling/Blueprints/Items/BP_SideScrollingPickup", "SideScrollingPickup"),
    ("/Game/Variant_SideScrolling/Blueprints/Items/BP_SideScrollingSoftPlatform", "SideScrollingSoftPlatform"),
]

RESAVES = [
    "/Game/Variant_Combat/Anims/AM_ChargedAttack",
    "/Game/Variant_Combat/Anims/AM_ComboAttack",
    "/Game/Variant_Combat/UI/UI_LifeBar",
    "/Game/Variant_Combat/Blueprints/AI/EnvQuery_Evade",
    "/Game/Variant_Combat/Blueprints/AI/EnvQuery_Flank",
    "/Game/Variant_Combat/Blueprints/AI/ST_CombatEnemy",
    "/Game/Variant_Platforming/Anims/ABP_Manny_Platforming",
    "/Game/Variant_Platforming/Anims/AM_Dash",
    "/Game/Variant_SideScrolling/Anims/ABP_Manny_SideScroller",
    "/Game/Variant_SideScrolling/Blueprints/AI/ST_SideScrollingNPC",
    "/Game/Variant_SideScrolling/UI/UI_SideScrolling",
]


def migrate_blueprints():
    saved = []
    for asset_path, class_name in BLUEPRINT_FIXES:
        bp = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not bp:
            unreal.log_warning(f"MISSING {asset_path}")
            continue
        parent = _load_class(f"{NEW_SCRIPT}.{class_name}")
        if not parent:
            unreal.log_warning(f"NO CLASS {class_name}")
            continue
        unreal.BlueprintEditorLibrary.reparent_blueprint(bp, parent)
        if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
            saved.append(asset_path)
        unreal.log(f"OK {asset_path} -> {class_name}")
    return saved


def resave_stale_assets():
    for path in RESAVES:
        if unreal.EditorAssetLibrary.load_asset(path):
            unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
            unreal.log(f"Resaved {path}")


if __name__ == "__main__":
    migrate_blueprints()
    resave_stale_assets()
