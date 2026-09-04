# First Village architecture

## Product goal

Simulate a synthetic ancient society beginning with a small hunter-gatherer band. Characters should not be omniscient or scripted. They perceive locally, remember selectively, communicate imperfectly, form relationships, propose work, build physical structures, teach techniques, create institutions, and pass knowledge into later generations.

## Core separation

### Cognition layer
Astra-powered agents decide intentions, plans, speech, social judgments, teaching, experimentation and building proposals.

### Simulation layer
The world validates and executes reality: movement, time, physics, materials, injuries, production, weather, terrain, construction feasibility and resource depletion.

### Presentation layer
The browser is the rapid-prototyping lab. Unreal Engine is the target embodied world.

## Agent model

Each person owns:
- identity, age, role, traits and private goals
- health, hunger, thirst, fatigue and morale
- skills learned through experience
- private inventory
- private knowledge graph
- episodic memories
- beliefs that can be true, uncertain or false
- pairwise trust/affinity values
- current plan and task

No agent receives global world state.

## Emergent construction

Construction uses a constrained blueprint grammar instead of arbitrary generated code. Agents may invent new structures if they can express them through known primitives and available materials.

Example primitive vocabulary:
- post
- wall panel
- roof panel
- hearth
- pit
- platform
- fence
- ditch
- channel
- storage bin
- drying beam

A building validator checks technique prerequisites, materials, dimensions, terrain, collisions and plausible purpose. Once accepted, the browser renders a schematic structure and Unreal assembles modular meshes procedurally.

## Technology model

Avoid a purely game-like tech tree. Techniques should ultimately emerge from discoveries and successful experiments. The first browser milestone may use knowledge thresholds as scaffolding, but the target model records recipes and causal knowledge such as:

`clay + shaping + high heat -> fired vessel`

`seed + prepared soil + water + time -> cultivated crop`

Knowledge can spread through observation, teaching and oral tradition.

## Unreal mapping

### Character
- `ACivAgentCharacter`
- `UCivAgentMindComponent`
- `UCivNeedsComponent`
- `UCivKnowledgeComponent`
- `UCivRelationshipComponent`
- `UCivInventoryComponent`

### World
- `ACivWorldDirector`
- `UCivResourceSubsystem`
- `UCivSettlementSubsystem`
- `UCivTechnologySubsystem`
- `UCivHistorySubsystem`
- `UCivConstructionSubsystem`

### Execution
1. World Director schedules a cognition tick.
2. Mind component creates a private observation.
3. Backend requests one structured decision from the agent.
4. Validator accepts, modifies or rejects it.
5. StateTree/Smart Objects execute low-level behavior.
6. Consequences update world state and agent memory.
7. Witnesses receive observations according to proximity and attention.

## Scale strategy

Do not call an LLM every frame. Use event-driven cognition and hierarchical time scales:
- locomotion/animation: Unreal every frame
- needs: deterministic simulation ticks
- ordinary decisions: every few simulated minutes
- emergency decisions: interrupt-driven
- long-term planning: a few times per simulated day
- historical memory summarization: daily/seasonal

This keeps costs controlled while preserving autonomous behavior.

## Long-term systems

- kinship, births, aging and death
- oral history and myths
- property and ownership norms
- specialization and trade
- conflict and mediation
- councils, leaders and laws
- religion and ritual
- migration and neighboring settlements
- disease and medicine
- ecology and overexploitation
- warfare and diplomacy
- writing and record keeping
- multi-century historical simulation
