Magic comes in many different forms in many different games. There are many determining factors for how magic works:

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