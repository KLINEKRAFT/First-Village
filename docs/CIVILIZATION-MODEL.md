# Emergent Civilization Model

First Village should not behave like a conventional survival game's scripted tech tree. The simulation is built around **private knowledge, physical constraints, social transmission, experimentation, and institutional memory**.

## 1. Knowledge belongs to people
A fact can be objectively true in the world without being known by the settlement. A person may discover a river, game trail, clay deposit, medicinal plant, or construction technique and keep that knowledge private until they teach or demonstrate it.

Three distinct states matter:
- **World truth** — what is actually present.
- **Personal knowledge** — what a specific person has directly observed or learned.
- **Belief** — something a person has been told and may misunderstand.

This permits rumors, expertise, secrecy, forgotten knowledge, rediscovery, and oral tradition.

## 2. Technology is an outcome of experiments
Capabilities are not awarded because a hidden timer reached Day 50. People experiment because of needs and available materials. An experiment can fail, partially succeed, or produce a repeatable technique.

Example chain:
1. Meat repeatedly spoils.
2. A cook notices meat near smoke lasts longer.
3. The cook experiments with a covered smoky rack.
4. A successful result creates a preservation technique.
5. A builder later proposes a dedicated smokehouse.
6. Others copy the building.

The simulation may use probability internally, but the probability is conditioned by the agent's skills, observations, materials, prior experiments and social knowledge.

## 3. Building is creative but validated
Agents are allowed to propose structures that do not exist in a fixed building catalog. They describe purpose, footprint, material requirements and a vocabulary of safe construction primitives.

The world validator—not the language model—decides whether a proposal is physically legal.

Validation includes:
- materials exist and are known to the builder;
- materials are available in sufficient quantity;
- dimensions are bounded;
- terrain is buildable;
- required techniques are known;
- primitive types are allowed;
- structure effects are mapped to simulation systems rather than invented by the model.

Successful custom buildings become part of the settlement's architectural tradition and may be imitated later.

## 4. Generations change the problem
Founders possess firsthand memories of migration and discovery. Their children do not. A second-generation person knows the founding only through parents, stories, landmarks and rituals.

Therefore every historical event can evolve through layers:
- event;
- firsthand memory;
- retold account;
- communal story;
- named tradition;
- ritual or myth.

This is a deliberate mechanism, not cosmetic lore.

## 5. Institutions emerge from repeated behavior
Leadership, councils, ownership, craft specialization, ritual and defense should arise when repeated individual behavior becomes socially recognized.

Examples:
- High trust around one person can create informal leadership.
- Repeated group debates can create a council practice.
- A workshop repeatedly used by one skilled maker can create specialization.
- Repeated remembrance of founders can become ancestor ritual.
- Resource disputes can eventually produce norms around possession or sharing.

## 6. Astra/LLM responsibility
The agent model should determine **intention**, not world truth or physics.

Good model decisions:
- "I want to ask June where she found water."
- "We keep losing meat; I want to try drying it above smoke."
- "I want to build a storehouse uphill because rain flooded the last pit."
- "I don't trust Theo's claim about the berries, so I will test a small amount first."

Bad model authority:
- directly changing inventory;
- teleporting;
- declaring a technology discovered without an experiment;
- creating arbitrary executable code;
- granting a building effects the simulation does not support.

## 7. Unreal target
Unreal Engine owns navigation, terrain, collision, animation, construction primitives, resource quantities and validated state changes. Astra receives a private observation and returns an intention using the shared decision schema.

The browser simulation is the rules laboratory. Unreal is the embodied world.