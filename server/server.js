const express = require('express');
const app = express();
app.use(express.json({ limit: '256kb' }));

const ALLOWED_ACTIONS = new Set([
  'gather','talk','hunt','rest','sleep','build','teach','explore','craft','vote','eat','drink','heal','experiment'
]);
const ALLOWED_PRIMITIVES = new Set([
  'walls','roof','posts','hearth','storage_bins','drying_beams','fence','pit','platform','channel','doorway','work_surface'
]);
const ALLOWED_MATERIALS = new Set(['wood','stone','thatch','clay']);

function finiteInRange(value, min, max) {
  return Number.isFinite(value) && value >= min && value <= max;
}

function validateBlueprint(bp) {
  if (!bp || typeof bp !== 'object') throw new Error('Blueprint must be an object');
  if (typeof bp.name !== 'string' || !bp.name.trim() || bp.name.length > 60) throw new Error('Blueprint requires a short name');
  if (typeof bp.purpose !== 'string' || !bp.purpose.trim() || bp.purpose.length > 180) throw new Error('Blueprint requires a short purpose');
  if (!bp.footprint || !finiteInRange(bp.footprint.w, 8, 80) || !finiteInRange(bp.footprint.h, 8, 80)) throw new Error('Invalid footprint');
  if (!bp.materials || typeof bp.materials !== 'object') throw new Error('Blueprint requires materials');
  for (const [material, amount] of Object.entries(bp.materials)) {
    if (!ALLOWED_MATERIALS.has(material)) throw new Error(`Unsupported material: ${material}`);
    if (!finiteInRange(amount, 0, 100)) throw new Error(`Invalid material quantity: ${material}`);
  }
  if (!Array.isArray(bp.primitives) || bp.primitives.length > 12) throw new Error('Invalid primitive list');
  for (const primitive of bp.primitives) {
    if (!ALLOWED_PRIMITIVES.has(primitive)) throw new Error(`Unsupported primitive: ${primitive}`);
  }
  return bp;
}

function validateDecision(x) {
  if (!x || typeof x !== 'object' || !ALLOWED_ACTIONS.has(x.action)) throw new Error('Invalid action');
  if (typeof x.reason !== 'string' || !x.reason.trim() || x.reason.length > 600) throw new Error('Decision requires a reason');
  if (x.utterance != null && (typeof x.utterance !== 'string' || x.utterance.length > 400)) throw new Error('Invalid utterance');
  if (x.target_id != null && !Number.isInteger(x.target_id)) throw new Error('Invalid target_id');
  if (x.blueprint != null) validateBlueprint(x.blueprint);
  if (x.action === 'build' && !x.blueprint) throw new Error('Creative build action requires a blueprint');
  return x;
}

async function callAgentModel(observation) {
  /*
   * Provider adapter belongs here.
   * The model must receive ONLY the observation available to this specific person.
   * Require JSON output matching shared/agent-decision.schema.json.
   * Keep all provider credentials server-side.
   */
  const lowWater = observation?.settlement?.water < 8;
  return lowWater
    ? { action: 'gather', resource: 'water', reason: 'The communal water supply is critically low.' }
    : { action: 'explore', reason: 'No external Astra/model provider is configured, so the safe fallback is exploration.' };
}

app.get('/health', (_req, res) => res.json({ ok: true, project: 'first-village', schema: 1 }));
app.post('/api/blueprint/validate', (req, res) => {
  try { res.json({ ok: true, blueprint: validateBlueprint(req.body) }); }
  catch (error) { res.status(400).json({ ok: false, error: error.message }); }
});
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

module.exports = { validateBlueprint, validateDecision };