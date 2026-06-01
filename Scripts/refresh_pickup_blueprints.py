"""
refresh_pickup_blueprints.py — update existing weapon/ammo pickup BPs only.

Does NOT create assets, respawn actors, or touch coins.
Run after Live Coding compile:
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/refresh_pickup_blueprints.py"
"""

import unreal

PICKUP_DIR = "/Game/ThirdPerson/Pickups"

PICKUP_SPECS = {
    "BP_Pickup_Pistol": {
        "item_id": "Weapon_Pistol",
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_001.SM_pistol_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_01_Cue.Abstract_Pop_01_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Rifle": {
        "item_id": "Weapon_Rifle",
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_001.SM_rifle_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_02_Cue.Abstract_Pop_02_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Shotgun": {
        "item_id": "Weapon_Shotgun",
        "mesh": "/Game/Weapons_Free/Meshes/SM_shotgun_001.SM_shotgun_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_03_Cue.Abstract_Pop_03_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Sniper": {
        "item_id": "Weapon_Sniper",
        "mesh": "/Game/Weapons_Free/Meshes/SM_sniper_rifle_001.SM_sniper_rifle_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_04_Cue.Abstract_Pop_04_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Ammo_Pistol": {
        "item_id": "Ammo_Pistol",
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_drum_001.SM_pistol_drum_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_01_Cue.Abstract_Pop_01_Cue",
        "qty": 20,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Rifle": {
        "item_id": "Ammo_Rifle",
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_magazine_001.SM_rifle_magazine_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_02_Cue.Abstract_Pop_02_Cue",
        "qty": 20,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Shotgun": {
        "item_id": "Ammo_Shotgun",
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_drum_001.SM_pistol_drum_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_03_Cue.Abstract_Pop_03_Cue",
        "qty": 20,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Sniper": {
        "item_id": "Ammo_Sniper",
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_magazine_001.SM_rifle_magazine_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_04_Cue.Abstract_Pop_04_Cue",
        "qty": 20,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
}


def make_mesh_rotator(pitch, yaw, roll):
    return unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll)


def set_prop(cdo, names, value):
    for name in names:
        try:
            cdo.set_editor_property(name, value)
            return True
        except Exception:
            continue
    return False


def is_pie_active():
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    for name in ("is_playing_in_editor", "in_play_in_editor", "is_in_play_in_editor"):
        fn = getattr(ues, name, None)
        if callable(fn):
            return fn()
    return False


def main():
    if is_pie_active():
        unreal.log_error("[refresh_pickups] STOP PIE first, then run again.")
        raise SystemExit(1)

    updated = 0
    for bp_name, spec in PICKUP_SPECS.items():
        bp_path = f"{PICKUP_DIR}/{bp_name}"
        if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
            unreal.log_warning(f"[refresh_pickups] Missing {bp_path} — skip")
            continue

        bp_asset = unreal.load_asset(bp_path)
        gen_class = bp_asset.generated_class() if bp_asset else None
        if not gen_class:
            gen_class = unreal.load_class(None, f"{bp_path}.{bp_name}_C")
        if not gen_class:
            unreal.log_warning(f"[refresh_pickups] Could not load class for {bp_path}")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            continue

        set_prop(cdo, ("FallbackItemId", "fallback_item_id"), unreal.Name(spec["item_id"]))
        set_prop(cdo, ("FallbackQuantity", "fallback_quantity"), spec["qty"])
        mesh = unreal.load_asset(spec["mesh"])
        if mesh:
            s = spec["scale"]
            set_prop(cdo, ("WorldMesh", "world_mesh"), mesh)
            set_prop(cdo, ("WorldMeshScale", "world_mesh_scale"), unreal.Vector(s, s, s))
            rot = spec.get("rotation")
            if rot:
                set_prop(
                    cdo,
                    ("WorldMeshRotation", "world_mesh_rotation"),
                    make_mesh_rotator(rot[0], rot[1], rot[2]),
                )
            offset = spec.get("offset")
            if offset:
                set_prop(
                    cdo,
                    ("WorldMeshOffset", "world_mesh_offset"),
                    unreal.Vector(offset[0], offset[1], offset[2]),
                )
        sound = unreal.load_asset(spec["sound"])
        if sound:
            set_prop(cdo, ("PickupSound", "pickup_sound"), sound)

        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        unreal.EditorAssetLibrary.save_loaded_asset(bp_asset)
        unreal.log(f"[refresh_pickups] Updated {bp_path}")
        updated += 1

    unreal.log(f"[refresh_pickups] Done. Updated {updated} blueprints.")

    coin_path = "/Game/ThirdPerson/Coins/BP_TPSCoin"
    if unreal.EditorAssetLibrary.does_asset_exist(coin_path):
        coin_bp = unreal.load_asset(coin_path)
        coin_class = coin_bp.generated_class() or unreal.load_class(None, f"{coin_path}.BP_TPSCoin_C")
        if coin_class:
            coin_cdo = unreal.get_default_object(coin_class)
            set_prop(coin_cdo, ("FallbackItemId", "fallback_item_id"), unreal.Name("Coin"))
            set_prop(coin_cdo, ("FallbackQuantity", "fallback_quantity"), 1)
            sound = unreal.load_asset("/Game/Interface_And_Item_Sounds/Cues/Coins_01_Cue.Coins_01_Cue")
            if sound:
                set_prop(coin_cdo, ("PickupSound", "pickup_sound"), sound)
            unreal.BlueprintEditorLibrary.compile_blueprint(coin_bp)
            unreal.EditorAssetLibrary.save_loaded_asset(coin_bp)
            unreal.log("[refresh_pickups] Updated BP_TPSCoin inventory fields")


if __name__ == "__main__":
    main()
