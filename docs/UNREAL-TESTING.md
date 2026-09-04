# Testing First Village in Unreal Engine 5

This branch turns `unreal/` into a real C++ Unreal project skeleton.

## Requirements

- Unreal Engine 5.8 recommended.
- Visual Studio 2022 on Windows, or Xcode on macOS, with Unreal C++ toolchain support.
- Node.js if you also want to run the local agent API bridge.

The generated targets use `BuildSettingsVersion.Latest` and `EngineIncludeOrderVersion.Latest` so the project is not pinned to an older UE5 minor release.

## 1. Get the project

Clone/pull the repository and open:

`unreal/FirstVillage.uproject`

If Unreal asks to rebuild missing modules, choose **Yes**.

If the editor says the project was made for another engine installation, use **Switch Unreal Engine Version** / regenerate project files for your installed UE5 version.

## 2. Create/open an empty level

The first milestone intentionally does not commit a binary `.umap`. Create a new **Basic** or **Empty** level in the editor.

The global game mode is already configured as `CivVillageGameMode`.

Press **Play**.

At runtime the game mode generates:

- a large collision ground plane
- dynamic navigation bounds
- an observer `DefaultPawn`
- `CivWorldDirector`
- 12 visible `CivAgentCharacter` actors
- AI controllers that independently roam the navmesh
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

Unreal's `CivAgentApiSubsystem` is configured to POST private agent observations to:

`http://127.0.0.1:8787/api/decide`

The C++ AI controller still keeps a local roaming fallback so the 3D world remains testable without an external model provider. The next integration step is to serialize each person's private observation, request a structured decision, validate it, then execute it through Unreal navigation/resource/building actors.

## 5. Current visual quality

This is an engineering simulation milestone, not final art. Humans are visible placeholder bodies. Buildings are blockout primitive geometry. Resource nodes still need production meshes/materials.

The purpose of this milestone is to prove the full 3D simulation plumbing before replacing placeholders with ancient-human characters, foliage, landscape, Nanite assets, PCG vegetation and a production modular building kit.

## Expected first run

You should be able to fly around a flat test valley and watch twelve visible agents independently roam while procedural village structures stand around the settlement center. That verifies world generation, embodied agents, runtime navmesh, procedural construction, observer mode and the HTTP bridge needed for Astra-powered cognition.
