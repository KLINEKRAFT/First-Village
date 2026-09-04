# Testing First Village

## Fastest local test

Requirements:
- Node.js 20+
- npm

From the repository root:

```bash
npm install
npm test
npm start
```

Then open:

```text
http://localhost:8787/sim
```

The same process serves the API and the browser simulation, which avoids file:// browser restrictions and matches the architecture we will use for real agent calls.

## What to test in the simulation

1. Press **Start** and increase the speed.
2. Confirm people explore rather than instantly knowing all resource locations.
3. Watch discoveries appear in history / observer state.
4. Let the settlement run long enough to produce structures.
5. Confirm structures physically appear around the camp.
6. Watch for experiments, failed ideas, successful techniques and repeated construction traditions.
7. Inspect people to compare private memories and knowledge.
8. Run long enough for stable housing/food to allow births and second-generation cultural transmission.

## Backend smoke tests

Run:

```bash
npm test
```

This checks the server-side decision and building validators.

Health endpoint:

```text
http://localhost:8787/health
```

Expected response:

```json
{"ok":true,"project":"first-village","schema":1}
```

## Test blueprint validation manually

With the server running:

```bash
curl -X POST http://localhost:8787/api/blueprint/validate \
  -H "Content-Type: application/json" \
  -d '{
    "name":"Smokehouse",
    "purpose":"Preserve hunted meat",
    "footprint":{"w":32,"h":26},
    "materials":{"wood":12,"stone":5,"thatch":4},
    "primitives":["walls","roof","hearth","drying_beams"]
  }'
```

A valid blueprint returns `ok: true`.

## Current limitation

The browser simulation is still using its local cognition layer. `/api/decide` exists and is validated, but it is not yet driving every person in the browser. That is the next integration milestone.

The model/provider key must remain on the backend; never put it in `web/app.js` or any browser file.
