"""
setup_coins.py
Run from UE Editor Python console:
    py "D:/GameDev/Unreal Projects/TPS_005_git/setup_coins.py"

Creates BP_TPSCoin (child of ATPSPickup) with a coin-shaped mesh,
then places 5 instances in Lvl_ThirdPerson.
"""

import math
import unreal

BP_DIR   = "/Game/ThirdPerson/Coins"
BP_NAME  = "BP_TPSCoin"
BP_PATH  = f"{BP_DIR}/{BP_NAME}"
MESH     = "/Engine/BasicShapes/Cylinder"
# 5 coins in a ring, easy to run into while testing
RADIUS   = 400.0
HEIGHT   = 100.0
COUNT    = 5

# ── 1. Load parent class ──────────────────────────────────────────────────────

parent_class = unreal.load_class(None, "/Script/TPS_005_git.TPSPickup")
if not parent_class:
    unreal.log_error("[setup_coins] Could not load ATPSPickup. "
                     "Did Live Coding compile succeed?")
    raise SystemExit(1)

unreal.log("[setup_coins] ATPSPickup loaded OK")

# ── 2. Create Blueprint asset ─────────────────────────────────────────────────

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

if unreal.EditorAssetLibrary.does_asset_exist(BP_PATH):
    unreal.log(f"[setup_coins] {BP_PATH} already exists — skipping creation")
    bp_asset = unreal.load_asset(BP_PATH)
else:
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    bp_asset = asset_tools.create_asset(BP_NAME, BP_DIR, unreal.Blueprint, factory)

if not bp_asset:
    unreal.log_error(f"[setup_coins] Failed to create {BP_PATH}")
    raise SystemExit(1)

unreal.log(f"[setup_coins] Blueprint asset ready: {BP_PATH}")

# ── 3. Add StaticMeshComponent (coin shape) ───────────────────────────────────

sds = unreal.get_editor_subsystem(unreal.SubobjectDataSubsystem)
handles = sds.k2_gather_subobject_data_for_blueprint(bp_asset)

# Find the root handle
root_handle = None
for h in handles:
    data = sds.get_data(h)
    obj  = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
    if obj and unreal.SubobjectDataBlueprintFunctionLibrary.is_scene_component(data):
        if unreal.SubobjectDataBlueprintFunctionLibrary.is_root_component(data):
            root_handle = h
            break

mesh_already_added = any(
    unreal.SubobjectDataBlueprintFunctionLibrary.get_object(
        sds.get_data(h)
    ).__class__ == unreal.StaticMeshComponent
    for h in handles
    if unreal.SubobjectDataBlueprintFunctionLibrary.get_object(sds.get_data(h)) is not None
)

if root_handle and not mesh_already_added:
    params = unreal.AddNewSubobjectParams(
        parent_handle=root_handle,
        new_class=unreal.StaticMeshComponent,
        blueprint_context=bp_asset,
    )
    result     = sds.add_new_subobject(params)
    mesh_handle = result.new_handle
    mesh_data   = sds.get_data(mesh_handle)
    mesh_comp   = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(mesh_data)

    if mesh_comp:
        cylinder = unreal.load_asset(MESH)
        mesh_comp.set_editor_property("static_mesh", cylinder)
        # Flatten into a coin: half-size XY, thin Z
        mesh_comp.set_editor_property(
            "relative_scale3d", unreal.Vector(0.5, 0.5, 0.12)
        )
        unreal.log("[setup_coins] StaticMeshComponent added with Cylinder mesh")
    else:
        unreal.log_warning("[setup_coins] Component created but could not get template object")
else:
    unreal.log("[setup_coins] Mesh component already present or root not found — skipping")

# ── 4. Compile + save Blueprint ───────────────────────────────────────────────

unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
unreal.EditorAssetLibrary.save_asset(BP_PATH)
unreal.log(f"[setup_coins] Blueprint compiled and saved")

# ── 5. Place 5 coins in the current level ────────────────────────────────────

bp_class = unreal.load_asset(BP_PATH)
if not bp_class:
    unreal.log_error("[setup_coins] Could not load Blueprint class for spawning")
    raise SystemExit(1)

spawn_lib = unreal.EditorLevelLibrary

placed = 0
for i in range(COUNT):
    angle = (2.0 * math.pi * i) / COUNT
    loc   = unreal.Vector(
        RADIUS * math.cos(angle),
        RADIUS * math.sin(angle),
        HEIGHT,
    )
    actor = spawn_lib.spawn_actor_from_object(bp_class, loc)
    if actor:
        placed += 1
        unreal.log(f"[setup_coins]   coin {i+1}/{COUNT} at "
                   f"({loc.x:.0f}, {loc.y:.0f}, {loc.z:.0f})")
    else:
        unreal.log_error(f"[setup_coins]   failed to place coin {i+1}")

spawn_lib.save_current_level()
unreal.log(f"[setup_coins] Done — {placed}/{COUNT} coins placed, level saved.")
unreal.log("[setup_coins] Tip: open BP_TPSCoin and implement the "
           "'On Picked Up' event: Add 'Print String' + 'Destroy Actor'.")
