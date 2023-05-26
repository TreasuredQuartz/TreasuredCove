# TreasuredCove

Developed with Unreal Engine 5.

### Game Premise    
#### Game Story    
- A certain quest undertaken with four other compatriots ends poorly.
  - Time passes and areas are developed by these four entities.
    - One in the stars                    (Star Fox Homage)
    - One reliving death                  (Zombies Homage)
    - One under constant war              (FPS Homage)
    - One surrounded by monsters          (Monster Hunter X Pokemon Homage)
  - These areas can be visited at different periods of development.
  - There are more entities than these four and the game can be "won" after completing 7 of these stories.
    - DLC for YouTubers Markiplier and GameGrumps planned, but not actively being developed anymore (main game focused)

#### Gameplay
- Initially start in a small town. There are other NPCs who have randomly generated personalities that may help/hinder the player.
- There is a dynamic "help board" in which NPCs will post quests for the player (or other npcs) to help with.
  - Quest can be revenge on hostile creature or characters, gathering materials, or other political drama.
    - Rewards would increase player relationships and have relative benefits to shops (such as new items made from the new gathered materials)
- Dynamic Political Leadership and town expanding.
  - Player can "takeover" towns and rule over them. dictating development and policies.
- Dungeons can be revisited after completion and customized.
  - New items, enchantments, challenges, wealth, and enemies can be added.
  - Dungeons need a procedural component to facilitate multiple playthroughs.
- All aspects of the world must change over time.
  - Think of Sid Meyers' Civilization, but playable and experienceable.
    - The leader of towns dictate development, which the player may interface with.
    - Multiple different Towns ongoing development at once (At least 10 planned currently (3 main world, 7 in a dungeon))
- Rogue-like elements
  - Customizable classes and earnable skills/talents.
    - procedural skill system ("skillscape" a 3D explorable space to discover new procedurally generated skills)
  - Veriety of Jobs
  - limitations and different treatment based on race, gender, and other social definement aspects
- Dynamic temperature system
  - Spreading fire
  - Deformable Snow
  - Hot/Cold objects
  - Hot/Cold areas
    - Player takes damage when too cold or too hot.
    - Player experiences long term negative effects when too cold or too hot for too long.
- New "crafting" system Forge
  - Players use tools to define shapes to make items.
    - Some items can be used as binding agents to attach multiple shapes together.
  - Player Generated Weapons
  - Player defined blueprints of items
  - Metalwork, Woodwork, and more.
- Old crafting system Transactions
  - Minecraft like
    - Combine item A with item B grants item C
      - Specific shape/tool/station/shapelass recipies.
    - Use tools/stations to Morph item A into item B
      - Furnace, Brewing station, mixing bowl, etc.

### Player-Vs-Player
#### Gameplay

A combination of Star Wars Battlefront and old Call-Of-Duty titles.

Will feature a Create-A-Class system with 30 perks that each have their own challenges to achieve a Pro-Version.

[![Perk Selection Screen]()](https://youtu.be/-KVA4OhRrvk)

### Programming
#### Third-Party Plugins
- GameplayAbilities
- StarterContent
- AnimStarterPack
- SenseSystem
- CloseCombatAnims              (Unused)
- Zombie_Pack
- ProceduralBuildingGenerator   (Unused)
- PlayFab                       (Learning Multiplayer)
- EpicOnlineServices            (Learning Multiplayer)
- AdvancedSessions              (Learning Multiplayer)
- AdvancedSteamSessions         (Learning Multiplayer)
- RuntimeMeshComponentCommunity (Recently Integrated)
- SimplexNoise
- Megascans                     (UnrealEngine5 Default Integration)

#### Personal Plugins
- GameplayTowns                 (Alpha)
- PhysxVehicles                 (experimental- Abandoned)
- ProceduralBuilding            (experimental- replaced)
- ProceduralPlanet              (Alpha)
- VolumeDynamics                (Water/Weather)(experimental)

#### Project Core Classes:
##### GACharacter:
Intended parent class for all "creatures" in-game. Anything that can more, interact, speak, and otherwise.    
Typically, players would activate abilities on their character unless they are holding an item.    
In which case the item's abilities would take precedence over the characters'.    
- Has an AbilitySystemComponent.
- 
- Has a default "PlayerCharacter" Blueprint child.

##### GAActor:
Intended parent class for all "items" or "interactibles" in-game. Anything that can be picked up, used, or interacted with by the GACharacters.
- Has an AbilitySystemComponent.
- Has multiple subclasses: GAWeapon, GAPotion, etc.

#### Server Side Classes (Gameplay):
GACharacterMovementComponent    
InventoryComponent    
RepositoryComponent    
LaunchingComponent    
TeamComponent    
ComboComponent    

#### Client Side Classes (For visuals only):
FloatingTextComponent    
FloatingBarComponent    
MovementTrailComponent    
FloatingItemInfoComponent    
ProjectilePathComponent    
FootprintComponent    


