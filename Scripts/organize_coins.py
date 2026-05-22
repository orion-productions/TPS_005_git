"""
Organize BP_TPSCoin: content folder + level outliner folder.

Run from UE Python console (editor open, level loaded):
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/organize_coins.py"

- Content: /Game/ThirdPerson/Coins/BP_TPSCoin
- Outliner: "Coins" folder (sibling of Lighting, Playground)
"""
import unreal

OLD_BP = "/Game/ThirdPerson/Blueprints/BP_TPSCoin"
NEW_BP = "/Game/ThirdPerson/Coins/BP_TPSCoin"
COIN_FOLDER = "Coins"


def organize_coins():
    unreal.EditorAssetLibrary.make_directory("/Game/ThirdPerson/Coins")

    if unreal.EditorAssetLibrary.does_asset_exist(OLD_BP):
        ok = unreal.EditorAssetLibrary.rename_asset(OLD_BP, NEW_BP)
        unreal.log(f"Moved blueprint: {OLD_BP} -> {NEW_BP} ({ok})")
    elif unreal.EditorAssetLibrary.does_asset_exist(NEW_BP):
        unreal.log(f"Blueprint already at {NEW_BP}")
    else:
        unreal.log_error("BP_TPSCoin not found")
        return

    with unreal.ScopedEditorTransaction("Move BP_TPSCoin actors into Coins folder"):
        count = 0
        for actor in unreal.EditorLevelLibrary.get_all_level_actors():
            if "BP_TPSCoin" in actor.get_class().get_name():
                actor.set_folder_path(COIN_FOLDER)
                count += 1
        unreal.log(f"Placed {count} coin actors in outliner folder '{COIN_FOLDER}'")

    coin_class = unreal.load_class(None, NEW_BP + ".BP_TPSCoin_C")
    if not coin_class:
        unreal.log_error("Failed to load BP_TPSCoin_C from new path")
        return

    unreal.EditorLoadingAndSavingUtils.save_dirty_packages(True, True)
    unreal.log("Coins organized and saved.")


if __name__ == "__main__":
    organize_coins()
