"""
Aim stance setup helper for TPS_005_git.

Duplicates ABP_Unarmed -> ABP_ThirdPerson_Aim (if missing), assigns it to
BP_ThirdPersonCharacter, and optionally reparents to UTPSAnimInstance after a C++ compile.

The AnimGraph layering (locomotion base + upper-body ADS) is authored in
/Game/ThirdPerson/Anims/ABP_ThirdPerson_Aim. Re-run this script after pulling
if the character reverts to ABP_Unarmed.

Run in UE: py "../../../../Unreal Projects/TPS_005_git/Scripts/setup_aim_stance_animbp.py"
"""
import unreal

SRC_ABP = "/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"
DST_ABP = "/Game/ThirdPerson/Anims/ABP_ThirdPerson_Aim"
CHAR_BP = "/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
PARENT_CLASS = "/Script/TPS_005_git.TPSAnimInstance"


def main():
    if not unreal.EditorAssetLibrary.does_asset_exist(DST_ABP):
        unreal.EditorAssetLibrary.duplicate_asset(SRC_ABP, DST_ABP)
        unreal.log(f"[AimStance] Duplicated {SRC_ABP} -> {DST_ABP}")

    char_bp = unreal.EditorAssetLibrary.load_asset(CHAR_BP)
    anim_class = unreal.load_class(None, DST_ABP + ".ABP_ThirdPerson_Aim_C")
    if not char_bp or not anim_class:
        unreal.log_error("[AimStance] Missing character BP or aim AnimBP generated class")
        return

    cdo = unreal.get_default_object(char_bp.generated_class())
    mesh = cdo.get_editor_property("mesh")
    if mesh:
        mesh.set_editor_property("anim_class", anim_class)
        unreal.EditorAssetLibrary.save_asset(CHAR_BP)
        unreal.log("[AimStance] Character Mesh AnimClass -> ABP_ThirdPerson_Aim")

    unreal.log(
        "[AimStance] Optional: reparent ABP to UTPSAnimInstance in editor "
        "(Class Settings) after Live Coding. C++ sets bIsAiming via reflection if needed."
    )


if __name__ == "__main__":
    main()
