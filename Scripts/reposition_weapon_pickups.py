"""
reposition_weapon_pickups.py — respawn weapon/ammo pickups with wider spacing.

Does NOT touch coins. Run with PIE stopped:
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/reposition_weapon_pickups.py"
"""

import importlib.util
import traceback
import unreal

SETUP_PATH = "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/setup_inventory.py"


def load_setup_module():
    spec = importlib.util.spec_from_file_location("setup_inventory", SETUP_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main():
    setup = load_setup_module()
    setup.require_editor_not_playing()
    setup.spawn_pickups_on_platforms()
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("[reposition_weapon_pickups] Done.")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        unreal.log_error(traceback.format_exc())
        raise
