"""
fix_weapon_orientation.py — push weapon mesh rotation to blueprints + refresh level actors.

Stop PIE first, then:
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/fix_weapon_orientation.py"
"""

import importlib.util
import traceback
import unreal

SETUP_PATH = "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/setup_inventory.py"
WEAPON_ROT = unreal.Rotator(pitch=-90.0, yaw=90.0, roll=90.0)


def load_setup_module():
    spec = importlib.util.spec_from_file_location("setup_inventory", SETUP_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    setup = load_setup_module()
    setup.require_editor_not_playing()

    weapon_bps = [
        "BP_Pickup_Pistol",
        "BP_Pickup_Rifle",
        "BP_Pickup_Shotgun",
        "BP_Pickup_Sniper",
    ]

    for bp_name in weapon_bps:
        bp_path = f"/Game/ThirdPerson/Pickups/{bp_name}"
        bp_asset = unreal.load_asset(bp_path)
        gen_class = bp_asset.generated_class() if bp_asset else None
        if not gen_class:
            continue
        cdo = unreal.get_default_object(gen_class)
        cdo.set_editor_property("world_mesh_rotation", WEAPON_ROT)
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        unreal.EditorAssetLibrary.save_loaded_asset(bp_asset)
        unreal.log(f"[fix_weapon_orientation] {bp_name} -> pitch=-90 yaw=90 roll=90")

    world = setup.require_editor_not_playing()
    refreshed = 0
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        label = actor.get_actor_label()
        if not label.startswith("Pickup_BP_Pickup_"):
            continue
        if "Ammo" in label:
            continue
        actor.set_editor_property("world_mesh_rotation", WEAPON_ROT)
        if hasattr(actor, "rerun_construction_scripts"):
            actor.rerun_construction_scripts()
        refreshed += 1

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"[fix_weapon_orientation] Refreshed {refreshed} weapon pickup actors.")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        unreal.log_error(traceback.format_exc())
        raise
