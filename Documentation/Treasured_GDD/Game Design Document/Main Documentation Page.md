# Working Title
___
## Internal Name
### Treasured Cove
### Title Contestants:
- Treasured Dungeons
- Dreams: By Brandon Biggs

# Current status
___
### Core gameplay systems completed: 2/18
- Ability System: 90%
- Skill Tree System: 90%
- Hex Grid System: 75%
- Gameplay Towns System: 25%
- Civilizations System: 5%
- Ecosystems System: 1%
- Items System: 55%
	- Current Content Amount: 1%
		- Barriers: 0
		- Ammunitions: 4
		- Bows: 1
		- Guns: 13
			- Assault Rifles: 2
			- Light Machine Guns: 2
			- Pistols: 3
			- Rifles: 1
			- Shotguns: 1
			- Submachine Guns: 2
			- Snipers: 1
		- Melee: 4
	- Inventory System: 90%
		- Needs testing
	- Equipment System: 90%
		- Needs testing
	- Repository System: 90%
		- Needs UI
	- Attachments System: 90%
		- Needs testing
	- Customization System: 15%
		- Needs In-Game tools
		- Needs assets
	- Crafting System: 15%
		- Needs recipes
		- Needs more items
		- Needs crafting materials
- World System: 50%
	- Weather: 75%
	- Fire Propagation System: 0%
	- Procedural Generation: 0%
		- After the base game has been beaten, the world should be able to be randomly generated with different dungeon types, dungeon locations, town locations, character races, character classes, crafting materials, creatures, etc.
	- Dungeons: 25%

### Core dungeons completed: 0/4
- Quartz Palace: 25%
- Mudkip Region: 1%
- Halios Castle: 0%
- Light Space: 1%

### Core mechanics completed: 15%
- Movement: 40%
	- Standing Gait: 100%
		1. Walking: 100%
		2. Jumping: 100%
		3. Running: 100%
	- Crouching Gait: 40%
		1. Sneaking: 50%
			- Needs separate sense stimuli
		2. Dodge Rolling: 0%
			- Hasn't been worked on.
		3. Sliding: 90%
			- Needs testing
			- Check sliding into corners...
	- Laying Gait: 0%
		1. Prone: 0%
		2. Dolphin Dive: 0%
		3. Prone Rolling: 0%
	- Climbing Gait: 70%
		1. Climbing: 75%
			- Needs animations.
		2. Wall-running: 90%
			- Needs animations.
		3. Vaulting: 75%
			- Needs animations.
			- Needs testing.
		4. Ledge Maneuvering: 50%
			- Needs testing.
	- Grappling Gait: 0%
		1. Swinging: 0%
		2. Pulling: 0%
		3. Climbing: 0%
		4. Roping: 0%
	- Swimming Gait: 30%
		1. Diving: 0%
		2. Ledge-Grabbing: 0%
		3. Swimming: 90%
			- Default Unreal Engine Implementation.
			- Consider replacing.
			- Needs Animations.

- Combat: 12%
	- Lock-on/Target system: 0%
	- Attributes: 55%
		1. Health: 100%
		2. Damage: 100%
		3. Death: 100%
		4. Unconsciousness: 0%
		5. Stamina: 50%
			- Needs testing
		6. Energy: 0%
			- Intended to be a total "Stamina" system based on daily usage.
			- Players go Unconscious (Or Immobile, undecided) when depleted.
		7. Mental Fortitude: 10%
			- Defense against charms, illusions, or other magics.
			- Only Mana has been implemented thus far.
		8. Magical Power: 90%
			- Nickname: Mana
			- Needs testing.
	- Combo System: 0%
		1. Frame data: 0%
		2. Consecutive inputs: 0%
		3. Input buffer (saving user inputs during actions): 0%
	- Enemies: 1%
		- Needs AI
		- Needs pathfinding (bugs out in the Practice map sometimes)
		- Needs unique meshes, sounds, textures, animations, etc (Assets)
		- Currently have only runtime-spawned zombies in the Quartz Palace dungeon.

- Dialogue: 10%
	- Not-Yet Third Party Dialogue Plugin used here.
	- No Dialogue content for NPCs has been developed as of yet.

# Concept Statement
___

# Genre
___

# Target Audience
___

# Concept Paragraph and Unique Selling Points
___

# Player Experience
___

# Art, sound and music
___

# Current Target Platform (and any system requirements)
___
Windows Personal Computers.

# Competition
___

# Monetization
___

[[Monetization]]

# Links to Detailed Sections
___

## All Design Pillars

[[Design Pillars]]
## Player objectives and progression

• *Who is the player in the game?*
They are an adventurer; someone spawned into the world after the initial inhabitants, and therefore used by them to complete tasks either too dangerous or too arduous for themselves. 

• *Synopsis of what the player knows when the game starts, and any following in-game
narrative*
The player is Incarnated by a magical well. This is the default manner in which NPCs and players spawn into the world. This well is the center of a town, as it would make sense for people to feel safe around this location, and be able to accrue future business interests through first impressions. They would be introduced into the mechanics of the world by the denizens of the town; the merchants of the town, the help posting board.

Their initial goal is helping the town, until the undertaking of a specific quest in which they join a group of adventures that gets party-wiped, including the player. This marks the end of the tutorial, and is the key moment defining the next steps the player takes.

• *Player’s primary goals and how they progress to them, including which parts are linear
or based on player choices*
The player's primary goal is to find dungeons in the world, and complete them. They can be completed in any order, and each one changes the others in unique ways after completion. This can be the form of: 
- New dialogue trees or paths due to new followers from the completed dungeon.
- More or less difficulty on specific parts (Generally dungeons should become progressively more difficult)
- General shift toward combat, puzzles, or dialogue.
Dungeons should generally be a new unique experience dependent on the order in which other dungeons are completed.

• Moment-by-moment gameplay (with links to UI section)
• core loops and outer loops
• reference back to Key Moments


## Game world

Backstory overview – important elements of the world fiction that set the stage for the
game, but do not directly affect the gameplay.
[[World]]
	o Link to full world fiction
• World organization: MVP levels, locations, zones, chapters – including introductory or
tutorial areas
• Game world physics: running, flying, falling, etc., -- anything of note for the design
• How does the player move through the game in terms of locations, chapters, etc.
• Easter Eggs planned, if any
User Interface
• Specifics of all needed control schemes (selection, activation, radial or context menu
use, tapping, dragging, etc.), ensuring that these are consistent across the game
• Screen flow mockups: each screen/menu in full functional detail (wireframe, no art),
showing the player’s flow through all aspects of the game.
• Mockups for on-screen controls (with links to individual features and systems)
• Help system details
• Game options: audio, visual, game save, etc.

## MVP Systems and Features

- Procedural Dungeons.
- [[In-Game Content Creation Tool Suite]]

• All major features such as character creation, crafting, combat, dialog, etc., including
areas for monetization (if any), and specific puzzles or similar
	o Intent, the “why” behind the feature
	o The moment-by-moment gameplay for each feature or system
	o The monetization aspects of each system or feature (if applicable)
	o Lots of pictures and diagrams – give the reader the feel for the feature or system
	o Links to technical docs for programmer consumption
		 Math, data structures, format of external data docs
		 Naming schemes for external directories and files
	o Break down into Epics and User Stories for implementation
• Include a description of the UI for each (with links back to the main UI section)



## Game Objects

• Listing of all NPCs and monsters, including
	o NPC backstory, narrative, dialog (links to specific dialog data files)
	o Attributes, attacks, special powers
• Game objects
	o Attributes and their values/ranges
	o Behaviors
• Link to external spreadsheet containing each object’s data and attribute information

[[Characters]]
[[Creatures]]

## Localization


# Tools
___

# Ideas and Expansions
___

# Unresolved Questions
___

# Prototypes
___
