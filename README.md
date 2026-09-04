# ASTRA // First Village

First Village is an autonomous ancient-civilization simulation: a small hunter-gatherer band enters an unfamiliar valley and, through private knowledge, cooperation, conflict, experimentation, construction, and generations of accumulated experience, can grow into a permanent village and eventually a civilization.

## Core design rule

**Agents choose intentions; the simulation owns reality.**

An agent may want to build, hunt, teach, explore, bargain, organize, worship, fortify, or invent something new. The world validates whether the action is physically possible, whether the required materials and techniques exist, and what actually happens.

## Current browser prototype

The web prototype is intentionally useful before Unreal integration. It models:

- autonomous hunter-gatherers with needs, roles, traits and private goals
- private resource knowledge and discovery
- knowledge transfer through conversation
- trust relationships and emergent leadership
- gathering, hunting, food spoilage, injuries and healing
- shared material stores
- agent-originated construction proposals
- village growth with structures placed into the world
- accumulated knowledge and technique emergence
- weather, farming, storage and demographic growth
- an observer view that can reveal hidden state

Open `web/index.html` directly for the local-brain version.

## Agent-created buildings

The long-term system does not restrict agents to a fixed menu of buildings. An AI agent may propose a constrained blueprint such as:

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

A deterministic validator checks materials, known techniques, footprint, site, allowed primitives and effect. This gives the agents architectural creativity without arbitrary code execution.

## Repository layout

- `web/` — browser simulation and observer UI
- `server/` — provider-neutral AI decision bridge
- `docs/ARCHITECTURE.md` — simulation and Unreal architecture
- `docs/AGENT-CONTRACT.md` — observation/action schema for Astra-powered agents

## Running locally

The browser simulation runs without an AI provider:

```bash
open web/index.html
```

For the development API bridge:

```bash
npm install
npm run dev
```

Then connect the simulation decision adapter to `POST /api/decide`.

## Unreal target

The browser and Unreal versions should share the same observation/action contract. Unreal will own navigation, animation, physics, Smart Objects, StateTrees, resources, procedural construction and world state. Astra-powered agents will own higher-level cognition, memory, communication, social reasoning and plans.

The objective is not a conventional colony game with scripted NPCs. The objective is to create a world where a synthetic society can develop its own history.