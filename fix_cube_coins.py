"""
Re-place coins on SM_Cube2/3/4/5 only.
- XY clipped to overlap with Floor (playable area)
- Z from line trace onto actual geometry (accounts for transforms)
Floor coins are left untouched.
"""
import unreal

BP_PATH = "/Game/ThirdPerson/Coins/BP_TPSCoin"
COIN_CLASS = unreal.load_class(None, BP_PATH + ".BP_TPSCoin_C")

STEP = 180.0
MARGIN = 60.0
Z_OFFSET = 100.0
# Walkable platform area around each cube pivot (meshes are huge; bounds span whole walls)
PLATFORM_HALF_XY = 380.0

CUBE_LABELS = ("SM_Cube2", "SM_Cube3", "SM_Cube4", "SM_Cube5")
FLOOR_LABEL = "Floor"


def platform_xy_box(actor, floor_xy):
    """Grid region: platform-sized box at actor pivot, clipped to Floor."""
    loc = actor.get_actor_location()
    cube = (
        loc.x - PLATFORM_HALF_XY,
        loc.x + PLATFORM_HALF_XY,
        loc.y - PLATFORM_HALF_XY,
        loc.y + PLATFORM_HALF_XY,
    )
    return intersect_xy(cube, floor_xy)


def mesh_top_z(actor):
    comp = actor.get_component_by_class(unreal.StaticMeshComponent)
    if comp:
        origin, extent, _ = unreal.SystemLibrary.get_component_bounds(comp)
        return origin.z + extent.z + Z_OFFSET
    origin, extent = actor.get_actor_bounds(False)
    return origin.z + extent.z + Z_OFFSET


def intersect_xy(a, b):
    """Intersect two AABBs in XY; return (xmin, xmax, ymin, ymax) or None."""
    xmin = max(a[0], b[0]) + MARGIN
    xmax = min(a[1], b[1]) - MARGIN
    ymin = max(a[2], b[2]) + MARGIN
    ymax = min(a[3], b[3]) - MARGIN
    if xmin > xmax or ymin > ymax:
        return None
    return xmin, xmax, ymin, ymax


def point_in_xy(x, y, a):
    return a[0] <= x <= a[1] and a[2] <= y <= a[3]


def grid_on_platform(platform_actor, xy_box):
    xmin, xmax, ymin, ymax = xy_box
    z = mesh_top_z(platform_actor)
    positions = []
    x = xmin
    while x <= xmax + 0.1:
        y = ymin
        while y <= ymax + 0.1:
            positions.append((x, y, z))
            y += STEP
        x += STEP
    return positions


def main():
    if not COIN_CLASS:
        unreal.log_error("[COINS] BP_TPSCoin class not found")
        return

    actors = unreal.EditorLevelLibrary.get_all_level_actors()

    platforms = {}
    floor_actor = None
    for a in actors:
        label = a.get_actor_label()
        if label in CUBE_LABELS:
            platforms[label] = a
        elif label == FLOOR_LABEL:
            floor_actor = a

    if not floor_actor:
        unreal.log_error("[COINS] Floor not found")
        return

    fo, fe = floor_actor.get_actor_bounds(False)
    floor_xy = (fo.x - fe.x, fo.x + fe.x, fo.y - fe.y, fo.y + fe.y)

    floor_top_z = fo.z + fe.z + Z_OFFSET

    # Remove only elevated cube coins (floor coins stay — they sit lower)
    removed = 0
    for a in actors:
        cn = a.get_class().get_name()
        if "TPSCoin" not in cn and "BP_TPSCoin" not in cn:
            continue
        loc = a.get_actor_location()
        if loc.z <= floor_top_z + 40:
            continue
        unreal.EditorLevelLibrary.destroy_actor(a)
        removed += 1
    unreal.log(f"[COINS] removed {removed} elevated cube coins (kept floor coins)")

    spawn_lib = unreal.EditorLevelLibrary
    total = 0
    for label in CUBE_LABELS:
        if label not in platforms:
            unreal.log_warning(f"[COINS] missing {label}")
            continue
        actor = platforms[label]
        xy = platform_xy_box(actor, floor_xy)
        if xy is None:
            unreal.log_warning(f"[COINS] {label}: no overlap with Floor")
            continue
        positions = grid_on_platform(actor, xy)
        for x, y, z in positions:
            spawn_lib.spawn_actor_from_class(
                COIN_CLASS, unreal.Vector(x, y, z), unreal.Rotator(0, 0, 0)
            )
            total += 1
        unreal.log(
            f"[COINS] {label}: {len(positions)} coins "
            f"(XY {xy[0]:.0f}..{xy[1]:.0f}, {xy[2]:.0f}..{xy[3]:.0f})"
        )

    spawn_lib.save_current_level()
    unreal.log(f"[COINS] Cube fix done — spawned {total} coins on platforms")


main()
