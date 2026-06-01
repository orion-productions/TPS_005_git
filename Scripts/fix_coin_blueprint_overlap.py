"""
fix_coin_blueprint_overlap.py — remove dangerous overlap/destroy nodes from BP_TPSCoin.

C++ now handles pickup + destroy. This clears Blueprint overlap events that can
fatal Cast to Actor on Default__Object during PIE streaming.

Run with PIE stopped:
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/fix_coin_blueprint_overlap.py"
"""

import unreal

COIN_BP = "/Game/ThirdPerson/Coins/BP_TPSCoin"
EVENTS_TO_REMOVE = (
    "ReceiveActorBeginOverlap",
    "ReceiveActorEndOverlap",
)


def is_pie_active():
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    for name in ("is_playing_in_editor", "in_play_in_editor", "is_in_play_in_editor"):
        fn = getattr(ues, name, None)
        if callable(fn):
            return fn()
    return False


def main():
    if is_pie_active():
        unreal.log_error("[fix_coin_bp] STOP PIE first.")
        raise SystemExit(1)

    if not unreal.EditorAssetLibrary.does_asset_exist(COIN_BP):
        unreal.log_error(f"[fix_coin_bp] Missing {COIN_BP}")
        raise SystemExit(1)

    bp_asset = unreal.load_asset(COIN_BP)
    removed = 0
    for event_name in EVENTS_TO_REMOVE:
        try:
            if unreal.BlueprintEditorLibrary.remove_graph(bp_asset, event_name):
                removed += 1
                unreal.log(f"[fix_coin_bp] Removed graph {event_name}")
        except Exception:
            pass

    unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
    unreal.EditorAssetLibrary.save_loaded_asset(bp_asset)
    unreal.log(f"[fix_coin_bp] Done. Removed {removed} overlap event graphs.")


if __name__ == "__main__":
    main()
