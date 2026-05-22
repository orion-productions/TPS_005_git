"""
Run from UE Python console or holodeck execute_python_code after opening the project.
Permanently maps K -> IA_Shoot on IMC_Default and saves the asset to disk.
"""
import unreal

IMC_PATH = "/Game/Input/IMC_Default"
SHOOT_PATH = "/Game/Input/Actions/IA_Shoot"


def fix_imc_shoot_k():
    imc = unreal.load_asset(IMC_PATH)
    shoot = unreal.load_asset(SHOOT_PATH)
    if not imc or not shoot:
        unreal.log_error(f"Missing assets: imc={imc} shoot={shoot}")
        return False

    data = imc.get_editor_property("default_key_mappings")
    maps = list(data.get_editor_property("mappings"))

    maps = [
        m
        for m in maps
        if not (
            m.get_editor_property("action")
            and m.get_editor_property("action").get_name() == "IA_Shoot"
        )
    ]

    key = unreal.Key()
    key.set_editor_property("key_name", "K")
    maps.append(unreal.EnhancedActionKeyMapping(action=shoot, key=key))

    data.set_editor_property("mappings", maps)
    imc.set_editor_property("default_key_mappings", data)
    imc.modify()

    saved = unreal.EditorAssetLibrary.save_asset(IMC_PATH, only_if_is_dirty=False)
    unreal.log(f"fix_imc_shoot_k: saved={saved} mappings={len(maps)}")
    return saved


if __name__ == "__main__":
    fix_imc_shoot_k()
