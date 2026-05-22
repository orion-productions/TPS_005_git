"""
Place BP_TPSCoin in a tight grid above Floor and SM_Cube2/3/4/5.
Run: py "D:/GameDev/Unreal Projects/TPS_005_git/setup_many_coins.py"
"""
import unreal

BP_PATH = "/Game/ThirdPerson/Coins/BP_TPSCoin"
COIN_CLASS = unreal.load_class(None, BP_PATH + ".BP_TPSCoin_C")

# Center-to-center spacing (sphere radius ~80; 180 is about as tight as is practical)
STEP = 180.0
MARGIN = 60.0
Z_ABOVE_SURFACE = 90.0

TARGET_LABELS = ("Floor", "SM_Cube2", "SM_Cube3", "SM_Cube4", "SM_Cube5")


def grid_positions(loc, extent):
    xmin = loc.x - extent.x + MARGIN
    xmax = loc.x + extent.x - MARGIN
    ymin = loc.y - extent.y + MARGIN
    ymax = loc.y + extent.y - MARGIN
    z = loc.z + extent.z + Z_ABOVE_SURFACE

    if xmin > xmax or ymin > ymax:
        return [(loc.x, loc.y, z)]

    out = []
    x = xmin
    while x <= xmax + 0.1:
        y = ymin
        while y <= ymax + 0.1:
            out.append((x, y, z))
            y += STEP
        x += STEP
    return out


def main():
    if not COIN_CLASS:
        unreal.log_error("[COINS] BP_TPSCoin class not found")
        return

    actors = unreal.EditorLevelLibrary.get_all_level_actors()

    # Remove existing coin pickups
    removed = 0
    for a in actors:
        cn = a.get_class().get_name()
        if "TPSCoin" in cn or "BP_TPSCoin" in cn:
            unreal.EditorLevelLibrary.destroy_actor(a)
            removed += 1
    unreal.log(f"[COINS] removed {removed} old coins")

    platforms = {}
    for a in actors:
        label = a.get_actor_label()
        if label in TARGET_LABELS:
            loc = a.get_actor_location()
            _origin, extent = a.get_actor_bounds(False)
            platforms[label] = (loc, extent)

    spawn_lib = unreal.EditorLevelLibrary
    total = 0
    for label in TARGET_LABELS:
        if label not in platforms:
            unreal.log_warning(f"[COINS] missing platform: {label}")
            continue
        loc, extent = platforms[label]
        positions = grid_positions(loc, extent)
        for x, y, z in positions:
            spawn_lib.spawn_actor_from_class(
                COIN_CLASS, unreal.Vector(x, y, z), unreal.Rotator(0, 0, 0)
            )
            total += 1
        unreal.log(f"[COINS] {label}: {len(positions)} coins")

    spawn_lib.save_current_level()
    unreal.log(f"[COINS] Done — spawned {total} coins total")


main()
