# Testing First Village in Unreal Engine 5

This branch turns `unreal/` into a real C++ Unreal project skeleton.

## Requirements

- Unreal Engine 5.6 is the current target for the generated Target.cs files.
- Visual Studio 2022 on Windows, or Xcode on macOS, with Unreal C++ toolchain support.
- Node.js if you also want to run the local agent API bridge.

## 1. Get the project

Clone/pull the repository and open:

`unreal/FirstVillage.uproject`

If Unreal asks to rebuild missing modules, choose **Yes**.

If your installed Unreal version differs from 5.6, right-click the `.uproject` and switch Unreal Engine version, then regenerate project files. If the generated Target.cs include-order enum differs in your engine, replace `EngineIncludeOrderVersion.Unreal5_6` with the version matching your install or `EngineIncludeOrderVersion.Latest`.

## 2. Create/open an empty level

The first milestone intentionally does not commit a binary `.umap`. Create a new **Basic** or **Empty** level in the editor.

The global game mode is already configured as `CivVillageGameMode`.

Press **Play**.

At runtime the game mode generates:

- a large collision ground plane
- dynamic navigation bounds
- an observer `DefaultPawn`
- `CivWorldDirector`
- 12 `CivAgentCharacter` actors
- AI controllers that roam the navmesh
- 18 resource nodes spanning water, food, wood, stone, clay and game
- a first hut
- a smokehouse
- a meeting shelter

The buildings are assembled from Unreal primitive meshes by `CivProceduralBuilding`.

## 3. Observer controls

- W/S — forward/back
- A/D — strafe
- E/Q — up/down
- mouse — look

## 4. Run the agent backend

From repository root:

```bash
npm install
npm test
npm start
```

The backend runs at `http://127.0.0.1:8787`.

Unreal's `CivAgentApiSubsystem` is configured to POST observations to:

`http://127.0.0.1:8787/api/decide`

The current C++ AI controller still uses local roaming so that the 3D world remains testable without a model provider. The next integration step is to have each controller periodically serialize its agent's private observation, call `CivAgentApiSubsystem`, validate the returned action, then execute it through navigation/resource/building actors.

## 5. Current visual quality

This is an engineering simulation milestone, not final art. Humans are visible placeholder primitive bodies. Buildings are blockout primitive geometry. Resource nodes currently require meshes/materials to be assigned for final visual identity.

The point of this milestone is to prove the full 3D simulation plumbing before replacing placeholders with MetaHuman/custom ancient characters, foliage, landscape, Nanite assets, PCG vegetation and production building kits.

## Expected first run

You should be able to fly around a flat test valley and watch twelve visible agents independently roam while procedural village structures stand around the settlement center. That verifies world generation, embodied agents, runtime navmesh, procedural construction and observer mode.
