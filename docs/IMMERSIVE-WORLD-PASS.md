# Immersive World Pass

This milestone converts the First Village Unreal prototype from a difficult-to-read engineering blockout into a legible prehistoric settlement while keeping all third-party art optional.

## What changed

### Observer experience
- purpose-built `CivObserverPawn`
- starts at a low cinematic overlook instead of far above the map
- altitude is clamped relative to terrain
- `C` or `Home`: return to settlement
- `Tab`: cycle through living villagers
- `F`: focus nearest villager
- HUD shows these controls at all times

### Villager readability
- visible human-shaped blockout bodies instead of one capsule
- role-based tunic colors
- animated arm/leg walk swing
- floating name + role labels at useful observer distances
- focused villagers receive a brighter/larger label
- all twelve founders begin clustered around the hearth so they are immediately discoverable

### Settlement readability
- central glowing First Village hearth/beacon
- warmer ancient-material colors for procedural buildings
- pitched thatch-like roof silhouettes
- foot-worn trail network connecting hut, smokehouse, meeting shelter and river ford
- clear generated river with visible banks

### Landscape
- terrain uses vertex-color biome shading instead of a white/gray debug surface when the engine vertex-color material is available
- woodland is denser away from the settlement clearing
- shrubs, reeds, grass, rocks and tree layers use HISM instancing
- reeds track the river corridor
- resources are spatially grouped by ecology rather than an arbitrary ring

### Simulation resilience
- Unreal no longer hammers an offline localhost server every decision cycle
- the API subsystem probes once, backs off while offline, and exposes status in the HUD
- villagers continue using deterministic survival behavior when the Node/Astra server is unavailable
- runtime navigation data is forced on and the runtime terrain uses synchronous collision cooking for a more reliable Recast build
- Lumen is disabled in the default project renderer settings for the Apple-silicon development profile, removing the persistent no-ray-tracing warning and reducing editor cost

## First test after merge

Because C++ and renderer settings changed, use a clean rebuild:

```bash
cd ~/Desktop/First-Village
git pull
rm -rf unreal/Binaries unreal/Intermediate unreal/Saved
open unreal/FirstVillage.uproject
```

Choose **Yes** if Unreal asks to rebuild missing modules.

Then press Play in an Empty level.

Expected first view:
1. camera is close enough to recognize the camp immediately
2. `FIRST VILLAGE` is visible above the hearth
3. population reads 12
4. founders are clustered near the settlement and have labels
5. Tab jumps directly from villager to villager
6. C/Home returns to camp
7. river, paths and biome regions are visually obvious
8. if the Node server is stopped, HUD says local fallback instead of flooding the Output Log with connection errors

## Local agent server

The Unreal simulation is playable without Node. For connected decisions:

```bash
cd ~/Desktop/First-Village
npm install
npm test
npm start
```

Then the HUD should change from local fallback to connected after a successful decision request.

## External art upgrade contract

The repository intentionally does not vendor Fab/Megascans/Mixamo binaries. Acquire those locally through their own services and preserve their license terms.

Recommended local content layout:

```text
/Game/FirstVillage/Art/
  Characters/
  Environment/
    Trees/
    Grass/
    Reeds/
    Rocks/
    Water/
  Materials/
    Ground/
    Wood/
    Clay/
    Thatch/
  Buildings/
    Modular/
  Props/
  Animals/
  Animations/
```

The code-generated visual fallback should remain even after production assets are integrated; it makes CI/repro/debug possible without proprietary or account-bound content.

See `docs/FREE-ASSET-SOURCES.md` for the researched source list.
