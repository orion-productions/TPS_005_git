# TPS_005_git

Third-person prototype built in **Unreal Engine 5.7** (TPS template). Orion Productions testbed for movement, physics interaction, and collectible gameplay.

## Features

- **Third-person character** with Enhanced Input (WASD, mouse look, jump)
- **Physics shooter** — press **K** to launch a physics ball from the character torso
- **Collectible coins** — `BP_TPSCoin` pickups with overlap collection; ball projectiles can collect multiple coins
- **Coin HUD** — on-screen count via `TPSCoinHUD` / `TPSCoinCountWidget`
- **Level organization** — coin actors grouped under a **Coins** outliner folder (alongside Lighting, Playground)
- **Template variants** — Combat, Platforming, and Side-scrolling sample modes included from the UE third-person template

## Requirements

- Unreal Engine **5.7** (engine association in `TPS_005_git.uproject`)
- Visual Studio 2022 with C++ game development workload (Windows)
- Optional: [Holodeck](https://github.com/orion-productions) editor MCP plugin (see `AdditionalPluginDirectories` in `.uproject`)

## Getting started

1. Clone the repository and open `TPS_005_git.uproject` in Unreal Editor.
2. Allow the editor to compile C++ modules on first launch.
3. Open level **`Lvl_ThirdPerson`** (`Content/ThirdPerson/Lvl_ThirdPerson`).
4. Press **Play** — move with WASD, jump with Space, shoot with **K**, collect gold coins.

## Project layout

| Path | Description |
|------|-------------|
| `Source/TPS_005_git/` | C++ game module |
| `Source/TPS_005_git/ThirdPerson/` | Pickup, physics shooter, coin HUD |
| `Content/ThirdPerson/` | Main level, character blueprints, input assets |
| `Content/ThirdPerson/Coins/` | `BP_TPSCoin` collectible blueprint |
| `Content/Input/` | Enhanced Input actions and mapping contexts |
| `Scripts/` | Editor Python helpers (coins, input, migration) |

## Editor scripts

Run from the UE **Python** console (`py "..."`):

- `Scripts/organize_coins.py` — move coin actors into the **Coins** outliner folder
- `Scripts/fix_imc_shoot_k.py` — persist **K → shoot** on `IMC_Default`
- `Scripts/migrate_tps_004_to_005.py` — reparent blueprints after project rename

## Controls (default)

| Action | Key |
|--------|-----|
| Move | W A S D |
| Look | Mouse |
| Jump | Space |
| Shoot physics ball | K |

## License

Private — Orion Productions. All rights reserved.
