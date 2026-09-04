# Testing First Village in Unreal Engine 5

This branch turns `unreal/` into a real C++ Unreal project and a runtime-generated 3D civilization test world.

## Requirements

- Unreal Engine 5.8 recommended.
- Visual Studio 2022 on Windows, or Xcode on macOS, with Unreal C++ toolchain support.
- Node.js 18+ for the local First Village agent bridge.
- Optional: an OpenAI API key with access to `gpt-6-astra`.

The generated targets use `BuildSettingsVersion.Latest` and `EngineIncludeOrderVersion.Latest` so the project is not pinned to one older UE5 minor release.

## 1. Get the project

Clone/pull the repository and open:

`unreal/FirstVillage.uproject`

If Unreal asks to rebuild missing modules, choose **Yes**.

If the editor says the project belongs to another engine installation, use **Switch Unreal Engine Version** / regenerate project files for your installed UE5 version.

## 2. Create/open an empty level

The project intentionally does not commit a large binary `.umap` yet. Create a new **Basic** or **Empty** level.

The global game mode is configured as `CivVillageGameMode`.

Press **Play**.

The game mode creates the world at runtime:

- a procedurally generated rolling valley with a low central basin and river corridor
- directional sunlight, skylight and height fog
- dynamic navigation bounds
- a flyable observer `DefaultPawn`
- `CivWorldDirector`
- 12 embodied `CivAgentCharacter` people
- private identity, memory, trust and known-fact state for every person
- survival needs: health, hunger, thirst, fatigue and morale
- 18 harvestable resource landmarks spanning water, food, wood, stone, clay and game
- authoritative communal food/water/material stores
- a first hut, smokehouse and meeting shelter assembled from procedural building primitives

## 3. Observer controls

- W/S — forward/back
- A/D — strafe
- E/Q — up/down
- mouse — look

## 4. Run without an API key

From the repository root:

```bash
npm install
npm test
npm start
```

The backend starts at `http://127.0.0.1:8787`.

`/api/decide` automatically uses a deterministic local fallback when `OPENAI_API_KEY` is not present. This is useful for validating the physical world and navigation before spending API tokens.

Check:

`http://127.0.0.1:8787/health`

The response should report `agent_provider: "local-fallback"`.

## 5. Run with GPT-6 Astra

Never place your API key inside Unreal source, browser JavaScript, or Git.

Set the key in your terminal before starting the Node bridge.

macOS/Linux:

```bash
export OPENAI_API_KEY="YOUR_KEY_HERE"
export OPENAI_MODEL="gpt-6-astra"
npm start
```

PowerShell:

```powershell
$env:OPENAI_API_KEY="YOUR_KEY_HERE"
$env:OPENAI_MODEL="gpt-6-astra"
npm start
```

Optional settings are shown in `.env.example`.

Check `/health` again. It should report the OpenAI provider and the selected model.

The server uses the OpenAI Responses API with strict JSON Schema output. Unreal never receives unrestricted model code. It receives one bounded decision that is validated before the world executes it.

## 6. What Astra can currently make a 3D person do

The current Unreal execution layer supports:

- explore — navigate to a reachable point
- gather — walk to a physical resource, harvest it, carry it back, and add it to communal stores
- hunt — travel to game, harvest food, and return it
- eat / drink — locate food or water and satisfy the person's need
- rest / sleep — recover fatigue
- talk — approach another person and alter relationship trust
- teach — approach another person and transmit one known fact
- heal — approach an injured person and improve health
- experiment / craft / vote — record a firsthand action/memory while deeper institution systems are developed
- build — parse an AI-proposed blueprint, validate primitives/materials/known techniques, consume real settlement materials, choose a site, snap it to terrain, and spawn a persistent procedural 3D structure

Gathering is spatial: material is not added to the village when the agent reaches the resource. The agent must successfully return to settlement first.

## 7. Agent cognition loop

Each person periodically constructs a private observation containing only information that person is allowed to know, plus currently visible nearby resources/people and public settlement stores.

The loop is:

`private observation -> First Village backend -> GPT-6 Astra -> strict structured decision -> world validator -> Unreal action -> experienced result -> personal memory`

High-level model decisions default to roughly every 35–70 seconds per idle person. Unreal handles locomotion, collision and physical task completion continuously between model calls.

If the backend is unavailable, the controller falls back to local survival/exploration behavior rather than freezing the simulation.

## 8. Current visual quality

This is the first functional 3D simulation milestone, not final art. Humans are placeholder primitive bodies, resources use primitive landmarks, and buildings are blockout procedural geometry.

The next visual pass should replace these with:

- ancient-human skeletal characters and animation
- a landscape/PCG vegetation layer
- trees, rocks, animals and water assets
- modular timber/thatch/clay/stone building pieces
- fires, smoke, weather and day/night lighting
- an observer HUD showing names, thoughts, stores and settlement history

## Expected first run

You should be able to fly over a generated valley and see twelve people moving independently among physical resources and an early settlement. With the Node bridge running, their high-level decisions can come from Astra; without a key, they remain locally testable.

### Verification note

The source has been assembled through the repository integration, but an actual Unreal Editor C++ compile cannot be executed from the ChatGPT tool environment. The first local editor build on your machine is therefore the required compile verification step. If Unreal reports a compiler error, copy the first error block into ChatGPT and fix it before chasing later cascading errors.
