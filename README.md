# ASTRA // First Village

First Village is an autonomous ancient-civilization simulation: a small hunter-gatherer band enters an unfamiliar valley and, through private knowledge, cooperation, conflict, experimentation, construction, oral tradition, and generations of accumulated experience, can grow into a permanent village and eventually a civilization.

## Core design rule

**Agents choose intentions; the simulation owns reality.**

An agent may want to build, hunt, teach, explore, organize, remember the dead, test an idea, or invent something new. The world validates whether the action is physically possible, whether required materials and techniques exist, and what actually happens.

## Current state

The development branch now contains a runnable browser simulation plus the first shared/Unreal architecture.

The browser prototype models:

- autonomous hunter-gatherers with needs, roles, traits and private goals
- private discovery and personal resource maps
- person-to-person knowledge transfer
- trust relationships and emergent leadership
- memories, hearsay and beliefs
- gathering, food spoilage, injuries and healing
- shared material stores
- experimental technique discovery with failed attempts
- agent-originated building proposals
- constrained custom structures such as smokehouses, fish weirs and storage pits
- physical village growth on the map
- families, households and second-generation births
- oral traditions and early institutions
- farming, storage, weather and demographic pressure
- browser save state
- observer view for hidden social/cultural state

Open `web/index.html` directly for the local-brain simulation.

## Agent-created buildings

The long-term system does not restrict agents to a fixed menu of buildings. An AI agent may propose a constrained blueprint:

```json
{
  "action": "build",
  "blueprint": {
    "name": "Smokehouse",
    "purpose": "preserve surplus meat",
    "footprint": {"w": 32, "h": 26},
    "materials": {"wood": 12, "stone": 5, "thatch": 4},
    "primitives": ["walls", "roof", "hearth", "drying_beams"]
  },
  "reason": "Too much hunted meat is spoiling before we can eat it."
}
```

A deterministic validator checks materials, known techniques, footprint bounds and allowed construction primitives. Unreal will additionally validate terrain, collision, site access and physical assembly.

Successful custom buildings can become part of the settlement's architectural tradition and later be copied by other builders.

## Technology and culture

There is no requirement that Day 50 unlock farming or Day 100 unlock masonry. Techniques are intended to emerge from experiments conditioned by need, available materials, prior observations, skill and luck.

Founders have firsthand memories. Later generations do not. Historical information can therefore evolve from event → memory → retelling → oral tradition → ritual or myth.

See `docs/CIVILIZATION-MODEL.md` for the full design.

## Repository layout

- `web/` — browser simulation and observer UI
- `server/` — provider-neutral AI decision bridge and validators
- `shared/agent-decision.schema.json` — shared structured decision contract
- `docs/ARCHITECTURE.md` — simulation and Unreal architecture
- `docs/AGENT-CONTRACT.md` — private observation/action contract
- `docs/CIVILIZATION-MODEL.md` — knowledge, experimentation, generations and institutions
- `unreal/Source/FirstVillage/` — initial UE5 C++ components and creative-building validator

## Running locally

The local browser brain needs no API provider:

```bash
open web/index.html
```

For the development agent bridge:

```bash
npm install
npm test
npm run dev
```

The API exposes:

- `GET /health`
- `POST /api/decide`
- `POST /api/blueprint/validate`

Never expose an Astra/model-provider credential in browser code. Browser or Unreal → First Village backend → agent provider.

## Unreal target

The browser and Unreal versions share the same observation/action philosophy. Unreal owns navigation, animation, physics, terrain, Smart Objects, StateTrees, resources, procedural construction and authoritative world state. Astra-powered agents own higher-level cognition, memory, communication, social reasoning and plans.

Initial UE5 source now includes:

- `UCivAgentMindComponent` — identity, private memories, trust and known facts
- `UCivBuildingValidator` — bounded blueprint/material/technique validation
- `FirstVillage.Build.cs` — module dependencies for JSON, AI and navigation

## Next milestones

1. Connect `/api/decide` to the chosen Astra/model provider with strict structured output.
2. Move browser cognition behind the same observation/action adapter used by Unreal.
3. Add conflict, exchange, possession norms and resource claims.
4. Add experiment histories so failed ideas can be remembered and iterated on.
5. Add procedural Unreal building assembly from validated primitives.
6. Create a UE5 valley test map with twelve embodied agents and Smart Objects.
7. Run long multi-generation simulations and preserve settlement histories for replay/analysis.

The objective is not a conventional colony game with scripted NPCs. The objective is to create a world where a synthetic society develops its own built environment, institutions, traditions and history.