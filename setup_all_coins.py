"""
Re-place all coins: 6 starter ring + floor grid + SM_Cube2-5 platforms.
XY from static mesh bounds (not actor pivot). Z from line trace on walkable surface.
"""
import math
import unreal

BP_PATH = "/Game/ThirdPerson/Coins/BP_TPSCoin"
COIN_CLASS = unreal.load_class(None, BP_PATH + ".BP_TPSCoin_C")

STEP = 180.0
MARGIN = 60.0
Z_ABOVE_SURFACE = 50.0
STARTER_COUNT = 6
STARTER_RADIUS = 400.0

CUBE_LABELS = ("SM_Cube2", "SM_Cube3", "SM_Cube4", "SM_Cube5")
FLOOR_LABEL = "Floor"


def comp_xy_box(actor):
    comp = actor.get_component_by_class(unreal.StaticMeshComponent)
    if comp:
        o, e, _ = unreal.SystemLibrary.get_component_bounds(comp)
    else:
        o, e = actor.get_actor_bounds(False)
    return (o.x - e.x, o.x + e.x, o.y - e.y, o.y + e.y)


def intersect_xy(a, b):
    xmin = max(a[0], b[0]) + MARGIN
    xmax = min(a[1], b[1]) - MARGIN
    ymin = max(a[2], b[2]) + MARGIN
    ymax = min(a[3], b[3]) - MARGIN
    if xmin > xmax or ymin > ymax:
        return None
    return xmin, xmax, ymin, ymax


def point_in_xy(x, y, box):
    return box[0] <= x <= box[1] and box[2] <= y <= box[3]


def platform_xy(actor, floor_xy):
    """Grid region = mesh bounds intersected with floor (visible platform)."""
    return intersect_xy(comp_xy_box(actor), floor_xy)


def trace_surface_z(world, x, y):
    hit = unreal.SystemLibrary.line_trace_single(
        world,
        unreal.Vector(x, y, 5000.0),
        unreal.Vector(x, y, -500.0),
        unreal.TraceTypeQuery.TRACE_TYPE_QUERY1,
        False,
        [],
        unreal.DrawDebugTrace.NONE,
        True,
        unreal.LinearColor(0, 0, 0, 0),
        unreal.LinearColor(0, 0, 0, 0),
        0.0,
    )
    if not hit:
        return None
    t = hit.to_tuple()
    if not t[0]:
        return None
    impact = t[4]
    return impact.z + Z_ABOVE_SURFACE


def main():
    if not COIN_CLASS:
        unreal.log_error("[COINS] BP_TPSCoin missing")
        return

    world = unreal.EditorLevelLibrary.get_editor_world()
    actors = unreal.EditorLevelLibrary.get_all_level_actors()

    floor_actor = None
    cube_actors = {}
    cube_boxes = []
    for a in actors:
        label = a.get_actor_label()
        if label == FLOOR_LABEL:
            floor_actor = a
        elif label in CUBE_LABELS:
            cube_actors[label] = a
            cube_boxes.append(comp_xy_box(a))
        elif label.startswith("SM_Cube"):
            cube_boxes.append(comp_xy_box(a))

    if not floor_actor:
        unreal.log_error("[COINS] Floor not found")
        return

    fo, fe = floor_actor.get_actor_bounds(False)
    floor_xy = (fo.x - fe.x, fo.x + fe.x, fo.y - fe.y, fo.y + fe.y)

    spawn = unreal.EditorLevelLibrary
    removed = 0
    for a in actors:
        cn = a.get_class().get_name()
        if "TPSCoin" in cn or "BP_TPSCoin" in cn:
            spawn.destroy_actor(a)
            removed += 1
    unreal.log(f"[COINS] removed {removed} existing coins")

    total = 0
    used = set()

    def spawn_at(x, y, z):
        if z is None:
            return False
        key = (int(x), int(y), int(z))
        if key in used:
            return False
        spawn.spawn_actor_from_class(
            COIN_CLASS, unreal.Vector(x, y, z), unreal.Rotator(0, 0, 0)
        )
        used.add(key)
        return True

    # 6 starter ring coins on walkable floor near origin
    starter = 0
    for i in range(STARTER_COUNT):
        angle = (2.0 * math.pi * i) / STARTER_COUNT
        x = STARTER_RADIUS * math.cos(angle)
        y = STARTER_RADIUS * math.sin(angle)
        z = trace_surface_z(world, x, y)
        if spawn_at(x, y, z):
            starter += 1
    unreal.log(f"[COINS] starter ring: {starter} coins")
    total += starter

    # Floor grid - skip XY inside any cube mesh bounds
    floor_n = 0
    x = floor_xy[0] + MARGIN
    while x <= floor_xy[1] - MARGIN:
        y = floor_xy[2] + MARGIN
        while y <= floor_xy[3] - MARGIN:
            if not any(point_in_xy(x, y, b) for b in cube_boxes):
                z = trace_surface_z(world, x, y)
                if spawn_at(x, y, z):
                    floor_n += 1
            y += STEP
        x += STEP
    unreal.log(f"[COINS] floor grid: {floor_n} coins")
    total += floor_n

    # Cube platforms - grid on mesh bounds footprint, Z from trace per point
    for label in CUBE_LABELS:
        actor = cube_actors.get(label)
        if not actor:
            continue
        xy = platform_xy(actor, floor_xy)
        if not xy:
            unreal.log_warning(f"[COINS] {label}: no platform XY")
            continue
        n = 0
        z_samples = []
        x = xy[0]
        while x <= xy[1] + 0.1:
            y = xy[2]
            while y <= xy[3] + 0.1:
                z = trace_surface_z(world, x, y)
                if z is not None:
                    z_samples.append(z)
                if spawn_at(x, y, z):
                    n += 1
                y += STEP
            x += STEP
        z_hint = f"{sum(z_samples)/len(z_samples):.0f}" if z_samples else "n/a"
        unreal.log(f"[COINS] {label}: {n} coins (avg Z~{z_hint})")
        total += n

    spawn.save_current_level()
    unreal.log(f"[COINS] Done - {total} coins placed")


main()
