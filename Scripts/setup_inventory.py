"""
setup_inventory.py — weapon/ammo pickups on quarter-cylinder platforms + coin via inventory.

WARNING: Stop PIE before running. Never run during Play-In-Editor.

Run after compiling TPSPickup / UTPSInventoryComponent (Live Coding or editor restart):
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/setup_inventory.py"

To update pickup blueprints ONLY (no spawn, no coins):
    py "D:/GameDev/Unreal Projects/TPS_005_git/Scripts/refresh_pickup_blueprints.py"
"""

import math
import traceback
import unreal

INV_DIR = "/Game/ThirdPerson/Inventory"
PICKUP_DIR = "/Game/ThirdPerson/Pickups"
COIN_BP = "/Game/ThirdPerson/Coins/BP_TPSCoin"

ITEM_TYPE_CURRENCY = 0
ITEM_TYPE_WEAPON = 1
ITEM_TYPE_AMMO = 2
WEAPON_NONE = 0
WEAPON_PISTOL = 1
WEAPON_RIFLE = 2
WEAPON_SHOTGUN = 3
WEAPON_SNIPER = 4

PICKUP_SPECS = {
    "BP_Pickup_Pistol": {
        "item_id": "Weapon_Pistol",
        "item_type": ITEM_TYPE_WEAPON,
        "weapon_family": WEAPON_PISTOL,
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_001.SM_pistol_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_01_Cue.Abstract_Pop_01_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Rifle": {
        "item_id": "Weapon_Rifle",
        "item_type": ITEM_TYPE_WEAPON,
        "weapon_family": WEAPON_RIFLE,
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_001.SM_rifle_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_02_Cue.Abstract_Pop_02_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Shotgun": {
        "item_id": "Weapon_Shotgun",
        "item_type": ITEM_TYPE_WEAPON,
        "weapon_family": WEAPON_SHOTGUN,
        "mesh": "/Game/Weapons_Free/Meshes/SM_shotgun_001.SM_shotgun_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_03_Cue.Abstract_Pop_03_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Sniper": {
        "item_id": "Weapon_Sniper",
        "item_type": ITEM_TYPE_WEAPON,
        "weapon_family": WEAPON_SNIPER,
        "mesh": "/Game/Weapons_Free/Meshes/SM_sniper_rifle_001.SM_sniper_rifle_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_04_Cue.Abstract_Pop_04_Cue",
        "qty": 1,
        "scale": 1.2,
        "rotation": (-90.0, 90.0, 90.0),
        "offset": (0.0, 0.0, 12.0),
    },
    "BP_Pickup_Ammo_Pistol": {
        "item_id": "Ammo_Pistol",
        "item_type": ITEM_TYPE_AMMO,
        "weapon_family": WEAPON_PISTOL,
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_drum_001.SM_pistol_drum_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_01_Cue.Abstract_Pop_01_Cue",
        "qty": 12,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Rifle": {
        "item_id": "Ammo_Rifle",
        "item_type": ITEM_TYPE_AMMO,
        "weapon_family": WEAPON_RIFLE,
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_magazine_001.SM_rifle_magazine_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_02_Cue.Abstract_Pop_02_Cue",
        "qty": 30,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Shotgun": {
        "item_id": "Ammo_Shotgun",
        "item_type": ITEM_TYPE_AMMO,
        "weapon_family": WEAPON_SHOTGUN,
        "mesh": "/Game/Weapons_Free/Meshes/SM_pistol_drum_001.SM_pistol_drum_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_03_Cue.Abstract_Pop_03_Cue",
        "qty": 8,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
    "BP_Pickup_Ammo_Sniper": {
        "item_id": "Ammo_Sniper",
        "item_type": ITEM_TYPE_AMMO,
        "weapon_family": WEAPON_SNIPER,
        "mesh": "/Game/Weapons_Free/Meshes/SM_rifle_magazine_001.SM_rifle_magazine_001",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Abstract_Pop_04_Cue.Abstract_Pop_04_Cue",
        "qty": 10,
        "scale": 5.0,
        "rotation": (90.0, 0.0, 0.0),
        "offset": (0.0, 0.0, 55.0),
    },
}

# One pickup centered on top of each anchor actor (Outliner labels).
WEAPON_PLACEMENTS = [
    ("BP_Pickup_Pistol", "SM_Cube"),
    ("BP_Pickup_Rifle", "SM_Cube 8"),
    ("BP_Pickup_Shotgun", "SM_Cube 10"),
    ("BP_Pickup_Sniper", "SM_Cube 12"),
]

AMMO_PLACEMENTS = [
    ("BP_Pickup_Ammo_Pistol", "SM_QuarterCylinder 1"),
    ("BP_Pickup_Ammo_Rifle", "SM_QuarterCylinder 2"),
    ("BP_Pickup_Ammo_Shotgun", "SM_QuarterCylinder 3"),
    ("BP_Pickup_Ammo_Sniper", "SM_QuarterCylinder 4"),
]

ACTOR_LABEL_ALIASES = {
    "SM_QuarterCylinder 1": "SM_QuarterCylinder",
    "SM_QuarterCylinder 2": "SM_QuarterCylinder2",
    "SM_QuarterCylinder 3": "SM_QuarterCylinder3",
    "SM_QuarterCylinder 4": "SM_QuarterCylinder4",
}


def ensure_directory(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def load_class(path):
    cls = unreal.load_class(None, path)
    if not cls:
        unreal.log_error(f"[setup_inventory] Missing class: {path}")
    return cls


def get_subobject_data_subsystem():
    try:
        return unreal.get_editor_subsystem(unreal.SubobjectDataSubsystem)
    except TypeError:
        get_fn = getattr(unreal.SubobjectDataSubsystem, "get", None)
        if get_fn:
            return get_fn()
    return None


def add_mesh_to_blueprint(bp_asset, mesh_path, scale=None):
    sds = get_subobject_data_subsystem()
    if not sds:
        unreal.log_warning("[setup_inventory] SubobjectDataSubsystem unavailable; mesh skipped on BP")
        return
    handles = sds.k2_gather_subobject_data_for_blueprint(bp_asset)
    root_handle = None
    mesh_comp = None
    for h in handles:
        data = sds.get_data(h)
        obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
        if not obj:
            continue
        if isinstance(obj, unreal.StaticMeshComponent):
            mesh_comp = obj
        elif unreal.SubobjectDataBlueprintFunctionLibrary.is_root_component(data):
            root_handle = h

    if not mesh_comp and root_handle:
        params = unreal.AddNewSubobjectParams(
            parent_handle=root_handle,
            new_class=unreal.StaticMeshComponent,
            blueprint_context=bp_asset,
        )
        result = sds.add_new_subobject(params)
        if result.new_handle:
            mesh_comp = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(sds.get_data(result.new_handle))

    mesh = unreal.load_asset(mesh_path)
    if mesh_comp and mesh:
        mesh_comp.set_editor_property("static_mesh", mesh)
        if scale:
            mesh_comp.set_editor_property("relative_scale3d", scale)


def _set_cdo_property(cdo, names, value):
    for name in names:
        try:
            cdo.set_editor_property(name, value)
            return True
        except Exception:
            continue
    unreal.log_warning(f"[setup_inventory] Could not set {names[0]} on {cdo.get_class().get_name()}")
    return False


def make_mesh_rotator(pitch, yaw, roll):
    return unreal.Rotator(pitch=pitch, yaw=yaw, roll=roll)


def configure_pickup_cdo(cdo, spec):
    _set_cdo_property(cdo, ("FallbackItemId", "fallback_item_id"), unreal.Name(spec["item_id"]))
    _set_cdo_property(cdo, ("FallbackItemType", "fallback_item_type"), spec["item_type"])
    _set_cdo_property(cdo, ("FallbackQuantity", "fallback_quantity"), spec["qty"])
    sound = unreal.load_asset(spec["sound"])
    if sound:
        _set_cdo_property(cdo, ("PickupSound", "pickup_sound"), sound)
    mesh = unreal.load_asset(spec["mesh"])
    if mesh:
        scale = spec["scale"]
        _set_cdo_property(cdo, ("WorldMesh", "world_mesh"), mesh)
        _set_cdo_property(
            cdo,
            ("WorldMeshScale", "world_mesh_scale"),
            unreal.Vector(scale, scale, scale),
        )
        rot = spec.get("rotation")
        if rot:
            _set_cdo_property(
                cdo,
                ("WorldMeshRotation", "world_mesh_rotation"),
                make_mesh_rotator(rot[0], rot[1], rot[2]),
            )
        offset = spec.get("offset")
        if offset:
            _set_cdo_property(
                cdo,
                ("WorldMeshOffset", "world_mesh_offset"),
                unreal.Vector(offset[0], offset[1], offset[2]),
            )


def create_pickup_blueprint(bp_name, spec, pickup_class, asset_tools):
    bp_path = f"{PICKUP_DIR}/{bp_name}"
    if not unreal.EditorAssetLibrary.does_asset_exist(bp_path):
        unreal.log_warning(f"[setup_inventory] Blueprint missing (will not create): {bp_path}")
        return None

    bp_asset = unreal.load_asset(bp_path)

    if not bp_asset:
        unreal.log_error(f"[setup_inventory] Failed blueprint: {bp_path}")
        return None

    scale = unreal.Vector(spec["scale"], spec["scale"], spec["scale"])
    add_mesh_to_blueprint(bp_asset, spec["mesh"], scale)

    cdo = unreal.get_default_object(bp_asset.generated_class())
    if cdo:
        configure_pickup_cdo(cdo, spec)

    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
    unreal.log(f"[setup_inventory] {bp_path}")
    return bp_path


def reparent_coin_pickup(pickup_class):
    if not unreal.EditorAssetLibrary.does_asset_exist(COIN_BP):
        unreal.log_warning(f"[setup_inventory] {COIN_BP} missing")
        return

    spec = {
        "item_id": "Coin",
        "item_type": ITEM_TYPE_CURRENCY,
        "weapon_family": WEAPON_NONE,
        "mesh": "/Engine/BasicShapes/Cylinder",
        "sound": "/Game/Interface_And_Item_Sounds/Cues/Coins_01_Cue.Coins_01_Cue",
        "qty": 1,
        "scale": 0.35,
    }

    bp_asset = unreal.load_asset(COIN_BP)
    try:
        unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, pickup_class)
    except Exception as exc:
        unreal.log_warning(f"[setup_inventory] Coin reparent skipped: {exc}")
    cdo = unreal.get_default_object(bp_asset.generated_class())
    if cdo:
        configure_pickup_cdo(cdo, spec)
    unreal.EditorAssetLibrary.save_asset(COIN_BP)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
    unreal.log("[setup_inventory] BP_TPSCoin -> ATPSPickup (inventory)")


def is_pie_active():
    ues = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    for name in ("is_playing_in_editor", "in_play_in_editor", "is_in_play_in_editor"):
        fn = getattr(ues, name, None)
        if callable(fn):
            return fn()
    return False


def require_editor_not_playing():
    if is_pie_active():
        unreal.log_error(
            "[setup_inventory] STOP PIE first (click Stop). "
            "Editor scripts must not run during Play-In-Editor."
        )
        raise SystemExit(1)

    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()
    if not world:
        unreal.log_error("[setup_inventory] No editor world. Stop PIE and try again.")
        raise SystemExit(1)
    return world


def find_actor_by_label(label):
    label = ACTOR_LABEL_ALIASES.get(label, label)
    world = require_editor_not_playing()
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        if actor.get_actor_label() == label:
            return actor
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        actor_label = actor.get_actor_label()
        if actor_label.replace(" ", "") == label.replace(" ", ""):
            return actor
    return None


def find_quarter_cylinder(label):
    return find_actor_by_label(label)


def actor_top_location(actor, z_padding=20.0):
    origin, extent = actor.get_actor_bounds(False)
    return unreal.Vector(origin.x, origin.y, origin.z + extent.z + z_padding)


def apply_mesh_to_spawned_actor(actor, mesh_path, scale):
    mesh = unreal.load_asset(mesh_path)
    if not mesh:
        return
    scale_vec = unreal.Vector(scale, scale, scale)
    try:
        for comp in actor.get_components_by_class(unreal.StaticMeshComponent):
            comp.set_editor_property("static_mesh", mesh)
            comp.set_editor_property("relative_scale3d", scale_vec)
            return

        root = actor.get_editor_property("root_component")
        smc = unreal.new_object(unreal.StaticMeshComponent, actor, "PickupMesh")
        if not smc or not root:
            return

        smc.set_editor_property("static_mesh", mesh)
        smc.set_editor_property("relative_scale3d", scale_vec)
        smc.attach_to_component(
            root,
            socket_name=unreal.Name(""),
            location_rule=unreal.AttachmentRule.KEEP_RELATIVE,
            rotation_rule=unreal.AttachmentRule.KEEP_RELATIVE,
            scale_rule=unreal.AttachmentRule.KEEP_RELATIVE,
            weld_simulated_bodies=False,
        )
        editor_actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        editor_actors.add_instance_component_to_actor(actor, smc)
        actor.modify()
    except Exception as exc:
        unreal.log_warning(f"[setup_inventory] Mesh on {actor.get_actor_label()}: {exc}")


def remove_existing_pickup_actors():
    world = require_editor_not_playing()
    editor_actor = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        if actor.get_actor_label().startswith("Pickup_"):
            editor_actor.destroy_actor(actor)


def spawn_pickups_on_platforms():
    remove_existing_pickup_actors()
    editor_actor = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    count = 0

    for bp_name, anchor_label in WEAPON_PLACEMENTS + AMMO_PLACEMENTS:
        anchor = find_actor_by_label(anchor_label)
        if not anchor:
            unreal.log_warning(f"[setup_inventory] Missing anchor: {anchor_label}")
            continue

        bp_path = f"{PICKUP_DIR}/{bp_name}"
        bp_class = unreal.load_class(None, f"{bp_path}.{bp_name}_C")
        if not bp_class:
            unreal.log_error(f"[setup_inventory] Class not found: {bp_path}")
            continue

        loc = actor_top_location(anchor)
        rot = unreal.Rotator(0.0, anchor.get_actor_rotation().yaw, 0.0)
        spawned = editor_actor.spawn_actor_from_class(bp_class, loc, rot)
        if spawned:
            spawned.set_actor_label(f"Pickup_{bp_name}")
            if hasattr(spawned, "rerun_construction_scripts"):
                spawned.rerun_construction_scripts()
            unreal.log(f"[setup_inventory] {bp_name} -> {anchor_label} at ({loc.x:.0f}, {loc.y:.0f}, {loc.z:.0f})")
            count += 1

    unreal.log(f"[setup_inventory] Spawned {count} pickups")


def refresh_weapon_pickup_blueprints(pickup_class, asset_tools):
    """Update weapon/ammo pickup BPs only (meshes + inventory fields). Does not touch coins or level actors."""
    for bp_name, spec in PICKUP_SPECS.items():
        create_pickup_blueprint(bp_name, spec, pickup_class, asset_tools)
    unreal.log("[setup_inventory] Refreshed weapon/ammo pickup blueprints.")


def main():
    require_editor_not_playing()

    pickup_class = load_class("/Script/TPS_005_git.TPSPickup")
    if not pickup_class:
        pickup_class = load_class("/Script/TPS_005_git.TPSItemPickup")
    if not pickup_class:
        unreal.log_error("[setup_inventory] Restart the editor after compiling, then run again.")
        raise SystemExit(1)

    test_cdo = unreal.get_default_object(pickup_class)
    has_inventory_fields = False
    if test_cdo:
        for prop in ("FallbackItemId", "fallback_item_id"):
            try:
                test_cdo.get_editor_property(prop)
                has_inventory_fields = True
                break
            except Exception:
                continue
    if not has_inventory_fields:
        unreal.log_error(
            "[setup_inventory] ATPSPickup missing inventory fields. "
            "Restart the editor after compile, then re-run."
        )
        raise SystemExit(1)

    ensure_directory(PICKUP_DIR)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    reparent_coin_pickup(pickup_class)

    for bp_name, spec in PICKUP_SPECS.items():
        create_pickup_blueprint(bp_name, spec, pickup_class, asset_tools)

    spawn_pickups_on_platforms()
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("[setup_inventory] Done.")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        unreal.log_error(traceback.format_exc())
        raise
