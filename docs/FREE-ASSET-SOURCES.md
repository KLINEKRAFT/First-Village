# First Village — researched free art and reference sources

This file tracks sources that are appropriate for upgrading the code-generated First Village blockout. Do not commit third-party binaries to this repository unless their license and redistribution terms explicitly permit it.

## Unreal / Fab

### Fab free content
https://www.unrealengine.com/fabfreecontent

Epic maintains an always-free catalog plus rotating limited-time free products, including Megascans. Use `Window > Fab` inside Unreal Engine 5.8 to acquire content into the local project.

Recommended searches:
- Megascans forest floor
- Megascans dirt path
- Megascans mossy rock
- Megascans cliff
- grass meadow
- reeds
- thatch
- wattle
- primitive hut
- campfire
- deer

### ElderBoom Hollow Massive Medieval Village Environment — currently free
https://www.fab.com/listings/e12de9d5-be28-40df-a387-42ae6f84e05c

Useful as a source/reference for rural buildings, nature, terrain, water, lighting, wells, walls, paths, gardens, wood and farm dressing. Its architectural language is later than First Village, so use selectively rather than dropping the complete medieval scene into the simulation.

## CC0 textures / HDRIs / models

### Poly Haven
https://polyhaven.com/
https://polyhaven.com/license

Poly Haven assets are CC0. Best use in First Village:
- bark
- dirt
- mud
- rock
- dry grass
- wood
- HDRI skies

## Character animation

### Adobe Mixamo
https://www.mixamo.com/

Useful starting animation set:
- idle
- walk
- jog
- carrying
- crouch / kneel
- chopping
- digging
- gathering / picking
- sitting
- conversational gestures

Final licensing should be checked against the terms shown for the user's Adobe/Mixamo account before distributing packaged source assets.

## Open-source Unreal research

### PCG Extended Toolkit
https://github.com/PCGEx/PCGExtendedToolkit

MIT-licensed toolkit/reference for more advanced Unreal PCG spatial operations, graphs, paths and asset management.

### Unreal PCG examples
https://github.com/freetimecoder/unreal-pcg-examples

Useful reference project for procedural content generation patterns.

### Community Mass Sample
https://github.com/Megafunk/MassSample

Useful future reference if First Village eventually scales from dozens to hundreds/thousands of simulated people using Unreal Mass Entity.

### Mass AI Example
https://github.com/Ji-Rath/MassAIExample

Useful reference for Mass movement, agent radius and LOD concepts.

## Art direction rule

First Village should not become a generic medieval fantasy village. The target is a believable early permanent settlement:
- earth, timber, reeds, fiber, clay and rough stone
- compact family/work clusters
- central hearth / meeting space
- foot-worn paths rather than roads
- small-scale storage and work areas
- visual evidence of experimentation and gradual permanence

The code-generated fallback visuals remain in the repository so the project is always runnable without third-party assets.
