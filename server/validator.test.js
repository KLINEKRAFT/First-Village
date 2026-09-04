const assert = require('assert');
const { validateBlueprint, validateDecision, localFallbackDecision, extractResponseText } = require('./server');

const smokehouse = {
  name: 'Smokehouse',
  purpose: 'Preserve hunted meat',
  footprint: { w: 32, h: 26 },
  materials: { wood: 12, stone: 5, thatch: 4, clay: 0 },
  primitives: ['walls', 'roof', 'hearth', 'drying_beams']
};

assert.equal(validateBlueprint(smokehouse).name, 'Smokehouse');
assert.equal(validateDecision({
  action: 'build',
  target_id: null,
  resource: null,
  utterance: null,
  blueprint: smokehouse,
  reason: 'Too much meat is spoiling.'
}).action, 'build');
assert.equal(validateDecision({
  action: 'explore',
  target_id: null,
  resource: null,
  utterance: null,
  blueprint: null,
  reason: 'I do not know the valley yet.'
}).action, 'explore');

assert.throws(() => validateBlueprint({ ...smokehouse, footprint: { w: 999, h: 26 } }), /footprint/i);
assert.throws(() => validateBlueprint({ ...smokehouse, primitives: ['nuclear_reactor'] }), /primitive/i);
assert.throws(() => validateDecision({ action: 'teleport', reason: 'Convenient.' }), /action/i);
assert.throws(() => validateDecision({ action: 'build', reason: 'I want shelter.' }), /blueprint/i);

const waterFallback = localFallbackDecision({ settlement: { water: 2, food: 20 } });
assert.equal(waterFallback.action, 'gather');
assert.equal(waterFallback.resource, 'water');
assert.equal(waterFallback.blueprint, null);
assert.equal(validateDecision(waterFallback).action, 'gather');

const outputText = extractResponseText({ output_text: '{"action":"explore"}' });
assert.equal(outputText, '{"action":"explore"}');
const nestedText = extractResponseText({ output: [{ content: [{ type: 'output_text', text: '{"action":"rest"}' }] }] });
assert.equal(nestedText, '{"action":"rest"}');

console.log('First Village validator tests passed.');
