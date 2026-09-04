const express = require('express');
const path = require('path');
const agentDecisionSchema = require('../shared/agent-decision.schema.json');

const app = express();
app.use(express.json({ limit: '256kb' }));

const ALLOWED_ACTIONS = new Set([
  'gather','talk','hunt','rest','sleep','build','teach','explore','craft','vote','eat','drink','heal','experiment'
]);
const ALLOWED_PRIMITIVES = new Set([
  'walls','roof','posts','hearth','storage_bins','drying_beams','fence','pit','platform','channel','doorway','work_surface'
]);
const ALLOWED_MATERIALS = new Set(['wood','stone','thatch','clay']);

const {
  $schema: _schemaUrl,
  $id: _schemaId,
  title: _schemaTitle,
  ...modelDecisionSchema
} = agentDecisionSchema;

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

function localFallbackDecision(observation) {
  const lowWater = Number(observation?.settlement?.water) < 8;
  const lowFood = Number(observation?.settlement?.food) < 8;
  if (lowWater) {
    return {
      action: 'gather', target_id: null, resource: 'water', utterance: null,
      reason: 'The communal water supply is critically low.', blueprint: null
    };
  }
  if (lowFood) {
    return {
      action: 'gather', target_id: null, resource: 'food', utterance: null,
      reason: 'The communal food supply is critically low.', blueprint: null
    };
  }
  return {
    action: 'explore', target_id: null, resource: null, utterance: null,
    reason: 'No external model is configured, so exploration is the safest useful fallback.', blueprint: null
  };
}

function extractResponseText(responseBody) {
  if (typeof responseBody?.output_text === 'string' && responseBody.output_text.trim()) {
    return responseBody.output_text;
  }
  for (const item of responseBody?.output || []) {
    for (const content of item?.content || []) {
      if (content?.type === 'output_text' && typeof content.text === 'string') {
        return content.text;
      }
    }
  }
  return '';
}

async function callAstra(observation) {
  const apiKey = process.env.OPENAI_API_KEY;
  if (!apiKey) return localFallbackDecision(observation);

  const model = process.env.OPENAI_MODEL || 'gpt-6-astra';
  const reasoningEffort = process.env.OPENAI_REASONING_EFFORT || 'low';

  const payload = {
    model,
    reasoning: { effort: reasoningEffort },
    store: false,
    instructions: [
      'You are one prehistoric human living inside the First Village simulation.',
      'Choose one immediate intention using only facts present in this private observation.',
      'You are not omniscient. Do not invent unseen resources, other people\'s private thoughts, or technologies the observation says you do not know.',
      'Prioritize survival, then relationships, useful experimentation, culture, and long-term settlement growth.',
      'A build action may creatively propose a structure, but use only the allowed blueprint primitives/materials and keep it plausible for an ancient village.',
      'The reason field is a short private rationale, not hidden chain-of-thought.'
    ].join(' '),
    input: JSON.stringify(observation),
    text: {
      format: {
        type: 'json_schema',
        name: 'first_village_agent_decision',
        strict: true,
        schema: modelDecisionSchema
      }
    }
  };

  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), Number(process.env.OPENAI_TIMEOUT_MS || 45000));
  try {
    const response = await fetch('https://api.openai.com/v1/responses', {
      method: 'POST',
      headers: {
        Authorization: `Bearer ${apiKey}`,
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(payload),
      signal: controller.signal
    });

    const body = await response.json().catch(() => ({}));
    if (!response.ok) {
      const message = body?.error?.message || `OpenAI request failed with ${response.status}`;
      throw new Error(message);
    }

    const text = extractResponseText(body);
    if (!text) throw new Error('Astra returned no structured decision text');
    return JSON.parse(text);
  } finally {
    clearTimeout(timeout);
  }
}

async function callAgentModel(observation) {
  return callAstra(observation);
}

app.get('/health', (_req, res) => res.json({
  ok: true,
  project: 'first-village',
  schema: 2,
  agent_provider: process.env.OPENAI_API_KEY ? 'openai' : 'local-fallback',
  model: process.env.OPENAI_API_KEY ? (process.env.OPENAI_MODEL || 'gpt-6-astra') : null
}));

app.post('/api/blueprint/validate', (req, res) => {
  try { res.json({ ok: true, blueprint: validateBlueprint(req.body) }); }
  catch (error) { res.status(400).json({ ok: false, error: error.message }); }
});

app.post('/api/decide', async (req, res) => {
  try {
    const result = await callAgentModel(req.body);
    res.json(validateDecision(result));
  } catch (error) {
    const status = error?.name === 'AbortError' ? 504 : 400;
    res.status(status).json({ error: error.message });
  }
});

const webRoot = path.resolve(__dirname, '..', 'web');
app.use(express.static(webRoot));
app.get('/sim', (_req, res) => res.sendFile(path.join(webRoot, 'index.html')));

const port = process.env.PORT || 8787;
if (require.main === module) {
  app.listen(port, () => {
    console.log(`First Village running at http://localhost:${port}`);
    console.log(`Simulation: http://localhost:${port}/sim`);
    console.log(`Health:     http://localhost:${port}/health`);
    console.log(`Agent brain: ${process.env.OPENAI_API_KEY ? (process.env.OPENAI_MODEL || 'gpt-6-astra') : 'local fallback'}`);
  });
}

module.exports = {
  app,
  validateBlueprint,
  validateDecision,
  localFallbackDecision,
  extractResponseText
};
