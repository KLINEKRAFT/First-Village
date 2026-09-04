# ASTRA // First Village

First Village is an autonomous ancient-civilization simulation. A small hunter-gatherer band enters an unfamiliar valley and, through private knowledge, cooperation, conflict, experimentation, construction, oral tradition and generations of accumulated experience, can grow into a permanent village and eventually a civilization.

## Core rule

**Agents choose intentions; the simulation owns reality.**

An agent can want to hunt, gather, teach, explore, heal, experiment, organize or invent a new building. Unreal validates and executes what is physically possible. The model never receives authority to run arbitrary code, spawn unlimited resources or directly alter world truth.

## Two simulation surfaces

### Browser society lab

The browser prototype remains the fastest way to test long-running civilization mechanics. It includes private discovery, memory/hearsay, trust, experimentation, creative building ideas, families, births, oral traditions and early institutions.

After `npm start`, open:

`http://localhost:8787/sim`

### Unreal Engine 3D world

`unreal/FirstVillage.uproject` is now a real C++ Unreal project.

The current 3D runtime generates:

- a rolling valley terrain at runtime, including a central basin and shallow river corridor
- sunlight, skylight, fog and dynamic navigation
- a flyable observer pawn
- 12 embodied people with private identities, memories, known facts and relationships
- health, hunger, thirst, fatigue and morale
- physical water, food, wood, stone, clay and game resource nodes
- authoritative communal food/water/material stores
- physical gathering: walk to resource -> harvest -> carry back -> deposit
- person-to-person talk, teaching and healing actions
- local survival fallback behavior if the agent backend is unavailable
- procedural 3D buildings assembled from validated primitive blueprints
- starter hut, smokehouse and meeting shelter
- an Unreal HTTP subsystem that talks to the same `/api/decide` endpoint as the browser architecture

See `docs/UNREAL-TESTING.md` for the full first-run guide.

## GPT-6 Astra cognition

The Node bridge can use OpenAI's GPT-6 Astra through the Responses API when `OPENAI_API_KEY` is set. Without a key it uses a deterministic local fallback, so the project remains testable without API spend.

The server requests strict structured output matching `shared/agent-decision.schema.json`. A decision looks like:

```json
{
  "action": "build",
  "target_id": null,
  "resource": null,
  "utterance": null,
  "reason": "Hunted meat is spoiling faster than we can eat it.",
  "blueprint": {
    "name": "Smokehouse",
    "purpose": "Preserve surplus meat",
    "footprint": {"w": 8, "h": 8},
    "materials": {"wood": 12, "stone": 5, "thatch": 4, "clay": 0},
    "primitives": ["walls", "roof", "hearth", "drying_beams"]
  }
}
```

The backend validates the response. Unreal validates it again against the settlement's actual stores, known techniques and supported construction primitives before consuming materials and creating anything.

## Creative building system

Agents are not permanently restricted to a menu of predefined building names. They may propose new structures using a constrained architectural vocabulary:

- walls
- roof
- posts
- hearth
- storage bins
- drying beams
- fence
- pit
- platform
- channel
- doorway
- work surface

This means an agent can eventually originate a smokehouse, storage pit, fish-processing shelter, workshop, ritual structure or other plausible village architecture while the simulation remains deterministic and safe.

## Running the local backend

Requires Node.js 18+.

```bash
npm install
npm test
npm start
```

Useful endpoints:

- `GET /health`
- `POST /api/decide`
- `POST /api/blueprint/validate`
- `GET /sim`

To enable Astra, set the API key in your terminal; never commit it:

```bash
export OPENAI_API_KEY="YOUR_KEY_HERE"
export OPENAI_MODEL="gpt-6-astra"
npm start
```

See `.env.example` for optional settings.

## Unreal project

Recommended current target: Unreal Engine 5.8. The build targets use the engine's `Latest` build/include settings instead of pinning the repository to one older UE5 minor release.

Open:

`unreal/FirstVillage.uproject`

Create an Empty/Basic level, allow Unreal to build the C++ modules, and press Play. `CivVillageGameMode` creates the simulation world at runtime, so no large binary map asset is required for this engineering milestone.

## Repository layout

- `web/` — browser civilization simulation and observer UI
- `server/` — Astra/local decision bridge and validators
- `shared/agent-decision.schema.json` — strict cross-runtime action contract
- `docs/` — architecture, civilization model and testing guides
- `unreal/` — UE5 project, world generation, embodied agents, resources, navigation and procedural construction

## Civilization architecture

The long-term information path is:

`world truth -> perception -> private memory/belief -> Astra intention -> validated Unreal action -> experienced consequence -> social transmission -> tradition/institution`

Founders can have firsthand memories. Descendants should inherit stories rather than magically inheriting world truth. Failed experiments should remain historical information that later people can reinterpret and improve.

## Next 3D milestones

1. Compile and play this branch in UE5 and fix any editor/toolchain-specific C++ issues.
2. Replace primitive humans with animated ancient-human characters.
3. Add PCG vegetation, trees, rocks, real water and animal actors.
4. Add task animation / Smart Objects for gathering, hearths, workbenches and building sites.
5. Port experimentation, households, births, oral tradition and institutions from the browser model into the authoritative Unreal world.
6. Add day/night, seasons, weather, disease and injury risk.
7. Add an observer HUD and historical replay/timeline.
8. Run multi-generation settlements where architecture, traditions and techniques are generated by the agents themselves.

The objective is not a conventional colony game with scripted NPCs. The objective is a synthetic ancient society that develops its own built environment, knowledge, institutions and history.
