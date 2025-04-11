#### Simple RPG Game
## Overview
This is a console-based Role-Playing Game (RPG) written in C++. Players can explore a map, fight enemies, loot chests, trade with NPCs, and manage their inventory. The game features a grid-based inventory system, item equipping, and a simple combat system.

## Features
Player Mechanics: Move around a map, level up, and manage stats (strength, agility, intelligence).
Items: Collect and equip weapons and armor with varying rarities (common, uncommon, magic, rare, legendary).
Combat: Fight enemies with a basic attack system based on weapon damage and player strength.
Inventory: 4x4 grid inventory for storing items, with options to inspect and equip.
Trading: Interact with NPCs to buy items like health potions, weapons, and armor.
Map: Dynamic map with chests, enemies, NPCs, and obstacles. Weather changes and new areas generate at map edges.
Non-blocking Input: Smooth gameplay with real-time key detection.

##Controls
W/A/S/D: Move the player (up/left/down/right).
F: Fight a nearby enemy.
L: Loot a nearby chest.
T: Trade with a nearby NPC.
I: Open inventory (then use 'I' to inspect, 'E' to equip, 'X' to exit).
