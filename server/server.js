const express = require('express');
const app = express();
app.use(express.json({ limit: '256kb' }));

const ALLOWED_ACTIONS = new Set([
  'gather','talk','hunt','rest','sleep','build','teach','explore','craft','vote','eat','drink','heal','propose_building'
]);
const ALLOWED_PRIMITIVES = new Set([
  'walls','roof','posts','hearth','storage_bins','drying_beams','fence','pit','platform','ditch','channel'
]);

function validateDecision(x) {
  if (!x || !ALLOWED_ACTIONS.has(x.action)) throw new Error('Invalid action');
  if (x.blueprint) {
    const bp = x.blueprint;
    if (!bp.name || !bp.purpose) throw new Error('Blueprint requires name and purpose');
    if (!bp.footprint || bp.footprint.w < 8 || bp.footprint.w > 100 || bp.footprint.h < 8 || bp.footprint.h > 100) throw new Error('Invalid footprint');
    for (const p of (bp.primitives || [])) if (!ALLOWED_PRIMITIVES.has(p)) throw new Error(`Unsupported primitive: ${p}`);
  }
  return x;
}

async function callAgentModel(observation) {
  // Provider-neutral placeholder. Replace with Astra/model-provider call.
  // Require strict JSON and never expose provider secrets to the browser.
  return { action: 'explore', reason: 'No external agent provider configured.' };
}

app.get('/health', (_req, res) => res.json({ ok: true, project: 'first-village' }));
app.post('/api/decide', async (req, res) => {
  try {
    const result = await callAgentModel(req.body);
    res.json(validateDecision(result));
  } catch (error) {
    res.status(400).json({ error: error.message });
  }
});

const port = process.env.PORT || 8787;
app.listen(port, () => console.log(`First Village agent bridge listening on http://localhost:${port}`));
