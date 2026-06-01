"""audit_pickups.py — list Pickup_* actors and platform anchors (PIE stopped)."""
import unreal

world = unreal.EditorLevelLibrary.get_editor_world()
if not world:
    unreal.log_error("[audit_pickups] No editor world")
    raise SystemExit(1)

actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
pickups = [a for a in actors if a.get_actor_label().startswith("Pickup_")]
unreal.log(f"[audit_pickups] PICKUP_COUNT={len(pickups)}")
for a in sorted(pickups, key=lambda x: x.get_actor_label()):
    loc = a.get_actor_location()
    hidden = a.is_hidden_ed()
    mesh = None
    for prop in ("WorldMesh", "world_mesh"):
        try:
            mesh = a.get_editor_property(prop)
            break
        except Exception:
            pass
    fid = None
    for prop in ("FallbackItemId", "fallback_item_id"):
        try:
            fid = a.get_editor_property(prop)
            break
        except Exception:
            pass
    cls = a.get_class().get_name() if a.get_class() else "?"
    unreal.log(
        f"[audit_pickups] {a.get_actor_label()} class={cls} "
        f"loc=({loc.x:.0f},{loc.y:.0f},{loc.z:.0f}) hidden_ed={hidden} mesh={mesh} item={fid}"
    )

anchors = [
    "SM_Cube",
    "SM_Cube8",
    "SM_Cube10",
    "SM_Cube12",
    "SM_QuarterCylinder",
    "SM_QuarterCylinder2",
    "SM_QuarterCylinder3",
    "SM_QuarterCylinder4",
]
for label in anchors:
    found = [a for a in actors if a.get_actor_label() == label]
    unreal.log(f"[audit_pickups] ANCHOR {label}: {'YES' if found else 'MISSING'}")
