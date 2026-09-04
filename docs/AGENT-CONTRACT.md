# Agent observation/action contract

The same contract should drive both the browser prototype and Unreal Engine.

## Observation

Only include information the character can reasonably know.

```json
{
  "agent_id": 3,
  "identity": {
    "name": "Caleb",
    "age": 29,
    "role": "Scout",
    "traits": ["bold", "curious"],
    "private_goal": "learn the valley"
  },
  "needs": {
    "health": 100,
    "hunger": 42,
    "thirst": 63,
    "fatigue": 25,
    "morale": 79
  },
  "inventory": {"food": 0, "water": 0, "wood": 0, "stone": 0},
  "known_places": [
    {"kind": "river", "distance": 190, "confidence": 1.0}
  ],
  "nearby_people": [
    {"id": 2, "name": "June", "health": 48, "task": "resting", "trust": 72}
  ],
  "memories": [
    "June treated my injury yesterday."
  ],
  "settlement_beliefs": {
    "food": "low",
    "water": "critical",
    "housing": "poor"
  },
  "known_techniques": ["fire", "stone_tools"],
  "allowed_actions": ["gather", "talk", "hunt", "rest", "build", "teach", "explore", "craft"]
}
```

## Ordinary action

```json
{
  "action": "gather",
  "resource": "water",
  "target_id": null,
  "utterance": null,
  "reason": "The common water supply is critically low."
}
```

## Creative building proposal

```json
{
  "action": "propose_building",
  "blueprint": {
    "name": "Smokehouse",
    "purpose": "preserve surplus meat",
    "footprint": {"w": 32, "h": 26},
    "materials": {"wood": 12, "stone": 5, "thatch": 4},
    "primitives": ["walls", "roof", "hearth", "drying_beams"],
    "desired_site": {
      "near": "camp",
      "far_from": "sleeping_area"
    }
  },
  "utterance": "We keep losing meat. I want to build a smoke shelter by the edge of camp.",
  "reason": "Preservation would turn hunting success into reliable stored food."
}
```

## Validator responsibilities

The model is allowed to be imaginative. The validator is not.

The validator must enforce:
- allowed action types
- known techniques
- material availability
- permitted construction primitives
- footprint limits
- terrain/buildability
- collision and spacing rules
- physically plausible effects
- rate limits and cooldowns

Rejected intentions should become experiences the agent can remember instead of silently disappearing.

## Memory

Store compact episodic events rather than raw transcripts forever. Important events receive salience scores. Periodically summarize older memories into longer-term beliefs and identity statements.

Recommended memory fields:
- timestamp
- subject(s)
- event type
- summary
- emotional valence
- salience
- confidence
- source: observed / told / inferred

This supports rumor and false belief because told or inferred memories need not equal objective world truth.
