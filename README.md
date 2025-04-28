# Text-Based RPG Adventure

A text-based role-playing game (RPG) developed in C++. Players explore a procedurally generated world, engage in turn-based combat, manage a dynamic inventory, trade with NPCs, and loot chests. The game features a rich progression system, item rarity tiers, and a dynamic weather system, providing an immersive console-based experience.

## Features
- **Dynamic World Map**: Procedurally generated maps with enemies (`E`), chests (`C`), NPCs (`N`), obstacles (`#`), and the player (`P`). Map size and complexity scale with player level.
- **Inventory Management**: 4x4 grid inventory (16 slots) with options to inspect, equip, use, drop, sort, move, and mark items as favorites (indicated by `#favorite` in item names).
- **Combat System**: Turn-based combat with enemies. Damage is calculated from the equipped weapon's damage and player's strength stat. Enemies counterattack, and defeated enemies yield experience and display an ASCII art death animation.
- **Trading with NPCs**: NPCs offer a shop with five items (health potions, weapons, armor) that scale with player level. Items are displayed in a table with name, rarity, price, and stats.
- **Items**:
  - **Health Potion**: Restores 30 HP (single-use, durability 1).
  - **Weapons**: Provide damage and strength bonuses (e.g., "Basic Sword: Damage: 20, Strength Bonus: +2").
  - **Armor**: Provide defense and agility bonuses (e.g., "Basic Armor: Defense: 5, Agility Bonus: +2").
  - **Generic Items**: Basic items like "Scrap" or "Junk" with minimal value.
- **Rarity System**: Five rarity tiers (Common, Uncommon, Magic, Rare, Legendary), each with distinct ANSI color coding in inventory and shops.
- **Weather System**: Maps feature dynamic weather (Sunny, Rainy, Cloudy) that changes every 10 actions, displayed in the map header.
- **Player Progression**: Gain experience from defeating enemies or looting chests. Leveling up increases max HP, strength, agility, intelligence, and unlocks larger maps with rarer items.
- **Save/Load System**: Save game state (stats, inventory, equipped items) to files in the `saves` directory. Load existing players or create new ones from the welcome screen.
- **Autosave**: Automatically saves progress every 10 actions.

## Requirements
- **C++ Compiler**: g++ or any compiler supporting C++11 or later.
- **Operating System**: Windows, Linux, or macOS.
- **Terminal**: Must support ANSI color codes for colored text display (e.g., red for enemies, yellow for chests). Windows Command Prompt may require a modern terminal like Windows Terminal.
- **Libraries**: Standard C++ libraries (`iostream`, `vector`, `string`, `filesystem`, etc.) and platform-specific libraries (`conio.h` for Windows, `termios.h` for Unix).

## Controls
- **Main Game**:
  - **Movement**: `W` (up), `A` (left), `S` (down), `D` (right).
  - **Fight**: `F` (attack nearby enemy).
  - **Loot**: `L` (open nearby chest).
  - **Trade**: `T` (open shop when near an NPC).
  - **Inventory**: `I` (open inventory menu).
  - **Save**: `V` (manually save game).
  - **Quit**: `Q` (save and exit).
- **Inventory Menu**:
  - **Move Cursor**: `W`/`A`/`S`/`D` (navigate 4x4 grid).
  - **Inspect**: `I` (view item details; press `F` to toggle favorite status).
  - **Equip**: `E` (equip weapon or armor, updating strength/agility stats).
  - **Use**: `U` (use item, e.g., health potion).
  - **Drop**: `Q` (drop non-favorite item).
  - **Sort**: `C` (sort inventory by rarity, descending).
  - **Move Item**: `M` (select item, move to new slot, press `M` to place or `X` to cancel).
  - **Exit**: `X` (return to main game).
- **Shop Menu** (when trading with NPC):
  - **Buy**: `B` followed by item number (`1`–`5`).
  - **Exit**: `X`.

## Game Structure
- **Classes**:
  - `Item`: Base class for all items with properties (name, price, durability, rarity, favorite status) and virtual methods (`display`, `use`, `serialize`).
  - `HealthPotion`: Derived from `Item`, restores 30 HP when used.
  - `Weapon`: Derived from `Item`, provides damage and strength bonuses.
  - `Armor`: Derived from `Item`, provides defense and agility bonuses.
  - `Player`: Manages HP, gold, level, experience, stats (strength, agility, intelligence), inventory, and equipped weapon/armor.
  - `Enemy`: Represents enemies with HP, damage, experience rewards, and position.
  - `NPC`: Manages shop inventory and trading mechanics.
  - `Map`: Generates and renders the game world with dynamic elements (enemies, chests, NPCs, obstacles, weather).
- **Files**:
  - `Game_inventory.cpp`: Main source file containing all game logic, classes, and the main loop.
- **Color Scheme** (using ANSI escape codes):
  - Player (`P`): Cyan (`\033[36m`).
  - Enemy (`E`): Red (`\033[31m`).
  - Chest (`C`): Gold (`\033[33m`).
  - NPC (`N`): Green (`\033[32m`).
  - Obstacle (`#`): Gray (`\033[90m`).
  - Rarity: Common (Gray, `\033[90m`), Uncommon (Green, `\033[32m`), Magic (Blue, `\033[34m`), Rare (Purple, `\033[35m`), Legendary (Yellow, `\033[33m`).
  - Messages: Success (Green), Warnings (Yellow), Combat (Red), Stats/Info (Cyan), Default (White, `\033[37m`).
- **Save Files**: Stored in the `saves` directory as `player_<name>.txt`, containing serialized player stats, equipped items, and inventory.

## Example Gameplay
1. Launch the game and select an existing player or create a new one on the welcome screen.
2. View the map with your character (`P`) at the center, surrounded by enemies (`E`), chests (`C`), NPCs (`N`), and obstacles (`#`).
3. Move (`W/A/S/D`) to explore, fight enemies (`F`), loot chests (`L`), or trade with NPCs (`T`).
4. Open the inventory (`I`) to equip better weapons/armor, use health potions, or organize items.
5. Trade with an NPC to purchase items like "Iron Sword" or "Health Potion" if you have enough gold.
6. Defeat enemies or loot chests to gain experience, leveling up to increase stats and unlock larger maps with rarer items.
7. Save progress manually (`V`) or rely on autosave (every 10 actions). Quit (`Q`) to save and exit.
8. If HP reaches 0, see a "Game Over" message, save progress, and exit.

## Notes
- The game uses ANSI color codes for a colorful interface. If colors are missing, verify your terminal supports ANSI escape sequences.
- Shop and chest items scale with player level:
  - Levels 1–2: Common, Uncommon items.
  - Levels 3–4: Common, Uncommon, Magic items.
  - Level 5+: All rarities (Common to Legendary).
- Inventory is capped at 16 slots. Excess items are discarded with a warning.
- Favorite items (marked with `F` in inspect mode) cannot be dropped, ensuring you keep valuable items.
- Comments in the source code are in Polish, but all in-game messages, prompts, and the README are in English.
- The game is cross-platform, with platform-specific input handling (`conio.h` for Windows, `termios.h` for Unix).
- Save files are stored in a `saves` directory created automatically in the game’s working directory.

## Future Improvements
- Add more item types (e.g., rings, mana potions).
- Implement advanced combat mechanics (e.g., critical hits, status effects).
- Introduce interactive map elements (e.g., traps, portals).
- Transition to a graphical interface using a library like SFML or SDL.
- Optimize save/load with binary file formats for faster I/O.
