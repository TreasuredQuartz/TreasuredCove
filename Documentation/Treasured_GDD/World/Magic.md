Magic comes in many different forms in many different games. There are many determining factors for how magic works:

## Avatar: The Last Airbender

Element manipulation as a type of magic system.

It is rooted in real-world physics and therefore influenced by real-world fighting styles.

The magic itself comes from the Spirit World, as spirits are intertwined with the world of the living.



## Video game depictions of magic:

- From within:
    - Stat-based.
    - Strength and Dexterity apply to raw magic strength and technique. (Avatar the last Airbender and physically-based magic.)
    - Constitution and Intelligence determine where you draw your magic from and how much. (Health or Mana pools.)
    - Wisdom and Charisma change the type of magic you can use. (DnD Wizard vs Rogue Trickster.)
- From without
    - Mana particles in the surrounding environment (air, liquids, and solids).
    - Planes of existence: Fire plane, Shadow plane, etc.
    - Entity-based: a higher being provides access.

Magic can be elemental or physical; it can be inherently good or evil. It is a quantity that can be stored and a quality that can be made.

Spells are often the way in which magic is given form; they may require words, gestures, or materials to perform. Spells instructions can be memorized, or written down.

Contracts, Enchantments, and Blessings can be performed by imbuing language with magic.

- Contracts that are magically bound cannot be broken;
- Enchantments bestowed upon objects will give access to magic;
- Blessings will impart a magical influence over a living participant's life.

Mana is also physical; this means that some users will have an innate magical connection and influence the strength or source of their spells with their bodies.

- Blood can be spent as a cost to produce magic.
- Technique and physical strength influence magical strength.
- Internal elemental predispositions can influence external magic produced.

Magical Strength Calculation:

```cpp
// Spell Gesture Somantic and Material requirements.
// Spell Caster Technique = The Caster's contextual flourish when casting.
SpellDiffMod = (SpellGesture + SpellSomantics + SpellMaterials) * SpellCasterTechnique;

// Spell Modification = The Spell's primary stat (From the Caster).
// Spell Caster Modification = The Caster's primary stat.
MagicalStrength = SpellMod * SpellCasterMod + SpellDiffMod;
```

Damage Calculation:

```cpp
// Spell Elemental Damage = Spell Caster's internal element or Spell's element (0 -> 1)
// Spell Damage Type = Spell's specific manner of causing damage (Slashing, Thrusting, Bludgening, etc) (0 -> 1)
// Elemental Resistance = Target's elemental resistance (is never 0)
// Damage Type Resistance = Target's damage type resistance (is never 0)
Damage = (MagicalStrength * ElementalTypeDamage * (1/ElementalResistance)) +
         (MagicalStrength * DamageTypeDamage * (1/DamageTypeResistance));
```

## Brandon Sanderson's laws on Magic Systems
### World building is less important compared to plot and characters.
1) Stories are plot, settings, and characters glued together by conflict.
2) Worldbuilding should be in service of story (plot and characters).

### Laws:
#### Magic must be understood
An author's ability to solve conflict with magic is ***directly proportional*** to how well the reader understands said magic.
1) ***Hard Magic System***: Reader can at least learn the rules, the rules are consistent, and the story revolves around utilizing magic like a science.
2) ***Soft Magic System***: Reader cannot understand the rules and it seems to have no internal logic.
3) ***External Logic***: Normally Hard Magic uses real world understanding of science to try and lend credibility to the magic system.
4) Use soft magic to emphasize strength of characters, not as a Dues Ex Machina
#### Limitations > powers. 
Characters are generally more interesting when dealing with what they can't do than what they can.
1) Example: It is more interesting to see Superman in a situation where his powers cannot help him than it is for us to see him beat someone up easily because he is simply so strong.
2) Often there is a cost to magic use. Economic cost (Spice in Dune), physical cost, intellectual cost (time it takes to learn), mystical costs.
3) Magic System "flaws" are what the character has to overcome.
4) Hindrance in a magic system: Lines you don't want to cross but could if you wanted to.
#### Expand what you already have before you add something new.
1) Example: it's more interesting to see the schism in religion between Judaism and Christianity than it is to see multiple different religions and rules.
2) Normally 2 elegant rules are more enjoyable to a reader than 30 shallow ones.
3) "You want to avoid having your worldbuilding being an inch deep."

## Treasured's Magic System

The term "Magic," refers to the powers granted by the primordial beings that reside separate from the world itself.

The strongest forms of "Magic" come from their sources:
- Oblivion: The embodiment of the concept of "nothing."
- Death: The embodiment of the concept of "loss."
- Judge: The final judge for actions taking during life.

All beings have an inherent amount of magic within them, but rarely do they have access to manipulating their own magic.

Living beings can reserve, replenish, and remove their own magic with enough time and learning.

Non-living beings can have magic instilled forcibly onto them, namely when a living being dies their magic disperses into the immediate surroundings.

***Nature Magic*** entails the manipulation of forces external to the person casting it, and is influenced by the magic within their environment. This is affected by the amount of magical beings that die within the ecosystem.
High population areas may not be highly magical areas, but previous warzone or mass gravesites are likely to be highly magically concentrated.

***Arcane Magic*** is personal control over one's own magical supply.

***Contract Magic*** is the governance of storing magic under conditions. 
A contract can be between a user and themselves, or a user and another user. 
Typically, this entails symbols imbued with magic and usually undertakes a very literal meaning of the symbols themselves. 
The symbols are usually the common language that the user(s) know, but may also be symbolic of what the user(s) understand it to be.
If a user(s) believe a different language or symbol to be more inherently magical than their common tongue, then the magic they impart on the symbol will be stronger.
Generally, a more ancient tongue

# Skill Tree

Quarter-Caster = 1 for 3
Half-Caster = 1 for 2
Full-Caster = 1 for 1

Spells can be invoked by name, either written or spoken.