# Unreal world polish milestone

This milestone makes the generated UE5 valley easier to read as a living place while preserving the code-generated/Git-friendly approach.

## Added

- `CivWorldClock` — accelerated simulation clock, day/night lighting, temperature and daily weather
- `CivHistorySubsystem` — authoritative chronological ledger for founding, construction and later major events
- `CivBiomeDresser` — HISM-based runtime tree and rock dressing with deterministic scattering
- founding/construction events recorded into settlement history

## Why history matters

History is world truth. Agent memory is personal truth.

A future generation should not receive the history subsystem directly. Instead, cultural transmission will sample historical events through living agents, oral retellings, monuments, rituals and records. That creates the chain:

`event -> eyewitness memory -> retelling -> tradition -> myth / institution`

## Visual asset strategy

The current tree/rock geometry deliberately uses Engine primitive meshes. It proves density, placement and runtime performance without committing large binary assets.

After editor verification, replace these through Blueprint subclasses/materials or production asset references while keeping `CivBiomeDresser` as the placement/runtime layer.

## Next visual milestone

1. skeletal ancient-human character class / animation Blueprint hook
2. locomotion, gather, carry, build, talk and sleep animation states
3. foliage/ground-cover material pass
4. visible river/water surface
5. wildlife actors and hunting behavior
6. weather particles/audio
7. observer HUD showing clock, stores, selected agent and history
