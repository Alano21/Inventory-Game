# Text-Based RPG Game

A simple text-based role-playing game (RPG) written in C++. The game features a dynamic world map, inventory management, combat, trading with NPCs, and lootable chests. Players can explore maps, fight enemies, collect items, and upgrade their character by equipping weapons and armor.

## Features
- **Dynamic Map**: Procedurally generated maps with enemies (`E`), chests (`C`), NPCs (`N`), obstacles (`#`), and the player (`P`). Map size and content scale with player level.
- **Inventory System**: 4x4 grid inventory with support for inspecting, equipping, using, and sorting items by rarity.
- **Combat**: Turn-based combat with enemies. Damage is based on the player's equipped weapon and strength stat.
- **Trading**: NPCs offer a shop with items (health potions, weapons, armor) that scale with player level. Items are displayed in a table with name, rarity, price, and stats.
- **Items**:
  - **Health Potion**: Restores 30 HP (single-use).
  - **Weapons**: Provide damage and strength bonuses (e.g., "Damage: 20, Strength Bonus: +2").
  - **Armor**: Provide defense and agility bonuses (e.g., "Defense: 5, Agility Bonus: +2").
- **Rarity System**: Items have five rarity tiers (Common, Uncommon, Magic, Rare, Legendary) with distinct colors in the shop and inventory.
- **Weather System**: Maps have dynamic weather (Sunny, Rainy, Cloudy) that changes periodically.
- **Player Progression**: Gain experience from defeating enemies or looting chests, level up to increase stats (strength, agility, intelligence) and max HP.

## Requirements
- **C++ Compiler**: g++ or any compiler supporting C++11.
- **Operating System**: Windows, Linux, or macOS.
- **Terminal**: Must support ANSI color codes for proper display of colored text (e.g., red enemies, golden chests).

## Controls
- **Movement**: `W` (up), `A` (left), `S` (down), `D` (right).
- **Fight**: `F` (attack nearby enemy).
- **Loot**: `L` (open nearby chest).
- **Trade**: `T` (open shop when near an NPC).
- **Inventory**: `I` (open inventory menu):
  - `I`: Inspect item.
  - `E`: Equip weapon or armor.
  - `U`: Use item (e.g., health potion).
  - `Q`: Drop item.
  - `C`: Sort inventory by rarity.
  - `X`: Exit inventory.
- **Shop**: When trading (`T`):
  - `1`–`N`: Select item to buy.
  - `X`: Exit shop.

## Game Structure
- **Classes**:
  - `Item`: Base class for all items (name, price, durability, rarity).
  - `HealthPotion`: Restores HP when used.
  - `Weapon`: Provides damage and strength bonuses.
  - `Armor`: Provides defense and agility bonuses.
  - `Player`: Manages HP, gold, stats, inventory, and equipped items.
  - `Enemy`: Represents enemies with HP, damage, and experience rewards.
  - `NPC`: Manages shop inventory and trading.
  - `Map`: Generates and displays the game world with dynamic elements.
- **Files**:
  - `Game_inventory.cpp`: Main source file containing all game logic.
- **Color Scheme**:
  - Player (`P`): Cyan (`\033[36m`).
  - Enemy (`E`): Red (`\033[31m`).
  - Chest (`C`): Gold (`\033[33m`).
  - NPC (`N`): Green (`\033[32m`).
  - Obstacle (`#`): Gray (`\033[90m`).
  - Rarity: Common (Gray), Uncommon (Green), Magic (Blue), Rare (Purple), Legendary (Yellow).
  - Messages: Success (Green), Warnings (Yellow), Combat (Red), Stats (Cyan).

## Example Gameplay
1. Start the game and see the map with your character (`P`) at the center.
2. Move (`W/A/S/D`) to explore, fight enemies (`F`), loot chests (`L`), or trade with NPCs (`T`).
3. Open the inventory (`I`) to equip better weapons/armor or use potions.
4. Visit the shop to buy items (e.g., Health Potion, Iron Sword).
5. Gain experience to level up, unlocking larger maps and rarer items.

## Notes
- The game uses ANSI color codes for a visually appealing interface. If colors don't display correctly, ensure your terminal supports ANSI codes.
- The shop's item selection scales with player level:
  - Levels 1–2: Common, Uncommon.
  - Levels 3–4: Common, Uncommon, Magic.
  - Level 5+: All rarities.
- Inventory is limited to 16 slots. Excess items are discarded.
- Comments in the code are in Polish, while all in-game messages are in English.
