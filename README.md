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

`unreal/FirstVillage.uproject` is a C++ Unreal Engine 5.8 project whose playable world is generated at runtime.

The current immersive 3D runtime includes:

- a rolling valley with an ecologically placed river corridor
- procedural sky atmosphere, sunlight, skylight, fog, day/night and weather
- biome-colored terrain with a visible settlement clearing
- instanced woodland, shrubs, grass, reeds and rocks
- a readable river and bank landmarks
- a central First Village hearth/beacon and foot-worn paths
- 12 embodied founders clustered around camp on first run
- role-colored human blockouts with heads, limbs, walking motion and floating name/role labels
- observer controls that make people easy to find: `C`/`Home` camp, `Tab` next villager, `F` nearest villager
- altitude clamping so the observer does not accidentally begin or drift into orbit
- health, hunger, thirst, fatigue and morale
- physical water, food, wood, stone, clay and game resources placed by ecology rather than an arbitrary ring
- authoritative communal food/water/material stores
- physical gathering: walk to resource -> harvest -> carry back -> deposit
- person-to-person talk, teaching and healing actions
- local survival fallback behavior when the agent backend is unavailable
- procedural ancient-material buildings assembled from validated primitive blueprints
- starter hut, smokehouse and meeting shelter
- dynamic runtime navigation and forced Recast rebuild
- an Unreal HTTP subsystem that talks to the same `/api/decide` endpoint as the browser architecture
- graceful backend-offline behavior with HUD status and request backoff instead of repeated connection-error spam

See `docs/IMMERSIVE-WORLD-PASS.md` for the clean rebuild/test flow and `docs/FREE-ASSET-SOURCES.md` for researched free art sources and Unreal reference projects.

## Observer controls

- `W A S D` — fly
- `Q / E` — descend / rise
- mouse — look
- `C` or `Home` — return to a cinematic settlement view
- `Tab` — cycle through living villagers
- `F` — focus the nearest living villager

The HUD shows these controls in-game.

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

Node is optional for the 3D simulation. If it is not running, villagers continue with local survival cognition and the Unreal HUD reports that state.

For connected decisions, Node.js 18+:

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

## First Unreal run after a C++ update

Recommended target: Unreal Engine 5.8.

```bash
cd ~/Desktop/First-Village
git pull
rm -rf unreal/Binaries unreal/Intermediate unreal/Saved
open unreal/FirstVillage.uproject
```

Choose **Yes** if Unreal asks to rebuild missing modules. Create/open an Empty level and press Play. `CivVillageGameMode` creates the world at runtime, so the repository does not need a large binary map asset.

## Free-art strategy

The repository deliberately keeps a complete code-generated fallback world so it remains cloneable, testable and reproducible without account-bound Marketplace files.

For the production art pass, acquire external assets locally and preserve their original license terms. The researched source list includes:

- Unreal/Fab free content and Megascans
- Poly Haven CC0 textures, HDRIs and models
- Adobe Mixamo animation sources
- open-source Unreal PCG and Mass examples

See `docs/FREE-ASSET-SOURCES.md` for details and a recommended `/Game/FirstVillage/Art/` folder structure.

## Repository layout

- `web/` — browser civilization simulation and observer UI
- `server/` — Astra/local decision bridge and validators
- `shared/agent-decision.schema.json` — strict cross-runtime action contract
- `docs/` — architecture, civilization model, asset research and testing guides
- `unreal/` — UE5 project, world generation, embodied agents, resources, navigation and procedural construction

## Civilization architecture

The long-term information path is:

`world truth -> perception -> private memory/belief -> Astra intention -> validated Unreal action -> experienced consequence -> social transmission -> tradition/institution`

Founders can have firsthand memories. Descendants should inherit stories rather than magically inheriting world truth. Failed experiments should remain historical information that later people can reinterpret and improve.

## Next major simulation milestones

1. Replace fallback founder blockouts with locally acquired skeletal ancient-human characters while keeping the same agent actor contract.
2. Add locomotion/work animation states and Smart Objects for hearths, gathering, building and social spaces.
3. Add true animal actors, hunting behavior and ecological regeneration.
4. Port households, births, aging, death and teaching into the authoritative Unreal world.
5. Port individual experimental discovery, rumor drift, oral tradition and institutions from the browser model.
6. Make paths respond to actual foot traffic and have architecture cluster by kinship/work/social relationships.
7. Add historical replay, selectable people, genealogy and settlement chronology.
8. Run multi-generation settlements where architecture, traditions and techniques are generated by the agents themselves.

The objective is not a conventional colony game with scripted NPCs. The objective is a synthetic ancient society that develops its own built environment, knowledge, institutions and history.
