const assert = require('assert');
const { validateBlueprint, validateDecision } = require('./server');

const smokehouse = {
  name: 'Smokehouse',
  purpose: 'Preserve hunted meat',
  footprint: { w: 32, h: 26 },
  materials: { wood: 12, stone: 5, thatch: 4 },
  primitives: ['walls', 'roof', 'hearth', 'drying_beams']
};

assert.equal(validateBlueprint(smokehouse).name, 'Smokehouse');
assert.equal(validateDecision({ action: 'build', blueprint: smokehouse, reason: 'Too much meat is spoiling.' }).action, 'build');
assert.equal(validateDecision({ action: 'explore', reason: 'I do not know the valley yet.' }).action, 'explore');

assert.throws(() => validateBlueprint({ ...smokehouse, footprint: { w: 999, h: 26 } }), /footprint/i);
assert.throws(() => validateBlueprint({ ...smokehouse, primitives: ['nuclear_reactor'] }), /primitive/i);
assert.throws(() => validateDecision({ action: 'teleport', reason: 'Convenient.' }), /action/i);
assert.throws(() => validateDecision({ action: 'build', reason: 'I want shelter.' }), /blueprint/i);

console.log('First Village validator tests passed.');