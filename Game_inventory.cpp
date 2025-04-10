#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <map>
#include <windows.h>
#include <conio.h>
using namespace std;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void setCursorPosition(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

enum Rarities { common, uncommon, magic, rare, legendary };
enum Weather { sunny, rainy, cloudy };

class Item {
protected:
    string name;
    float price;
    int durability;
    Rarities rarity;
public:
    Item() : name("Scrap"), price(0.1f), durability(100), rarity(common) {}
    Item(string name, float price, int durability, Rarities rarity)
        : name(name), price(price), durability(durability), rarity(rarity) {}

    virtual void display() const {
        setColor(15);
        cout << name << " (" << price << "g, Durability: " << durability << ", Rarity: ";
        switch(rarity) {
            case common: setColor(8); cout << "Common"; break;
            case uncommon: setColor(2); cout << "Uncommon"; break;
            case magic: setColor(9); cout << "Magic"; break;
            case rare: setColor(5); cout << "Rare"; break;
            case legendary: setColor(6); cout << "Legendary"; break;
        }
        setColor(15);
        cout << ")\n";
    }

    string getName() const { return name; }
    float getPrice() const { return price; }
    int getDurability() const { return durability; }
    Rarities getRarity() const { return rarity; }

    void setPrice(float newPrice) { price = newPrice; }
    void setDurability(int newDurability) { durability = newDurability; }
};

class Weapon : public Item {
private:
    float damage;
public:
    Weapon() : Item("Basic Sword", 10.0f, 100, common), damage(10.0f) {}
    Weapon(string name, float price, int durability, Rarities rarity, float damage)
        : Item(name, price, durability, rarity), damage(damage) {}

    void display() const override {
        Item::display();
        setColor(12);
        cout << "Damage: " << damage << "\n";
        setColor(15);
    }

    float getDamage() const { return damage; }
};

class Armor : public Item {
private:
    float defense;
public:
    Armor() : Item("Basic Armor", 20.0f, 100, common), defense(5.0f) {}
    Armor(string name, float price, int durability, Rarities rarity, float defense)
        : Item(name, price, durability, rarity), defense(defense) {}

    void display() const override {
        Item::display();
        setColor(11);
        cout << "Defense: " << defense << "\n";
        setColor(15);
    }

    float getDefense() const { return defense; }
};

class Enemy {
public:
    string name;
    float HP;
    float damage;
    int experience;

    Enemy(string n, float hp, float dmg, int exp) : name(n), HP(hp), damage(dmg), experience(exp) {}
    void attack(float& playerHP) const {
        playerHP -= damage;
        setColor(12);
        cout << name << " attacks! Player HP: " << playerHP << "\n";
        setColor(15);
    }
};

class Player {
private:
    string name;
    float HP;
    float maxHP;
    float gold;
    int level;
    int experience;
    int nextLevelExp;
    Weapon weapon;
    Armor armor;
    vector<Item> inventory;
    map<string, int> stats;

public:
    Player(string n) : name(n), HP(100.0f), maxHP(100.0f), gold(100.0f), level(1), experience(0), nextLevelExp(100) {
        stats["strength"] = 10;
        stats["agility"] = 10;
        stats["intelligence"] = 10;

        inventory.push_back(Weapon("Basic Sword", 10.0f, 100, common, 10.0f));
        inventory.push_back(Armor("Basic Armor", 20.0f, 100, common, 5.0f));
    }

    void showStats() const {
        setColor(10);
        cout << "Player: " << name << " | HP: " << HP << "/" << maxHP << " | Gold: " << gold << "\n";
        setColor(14);
        cout << "Level: " << level << " | Exp: " << experience << "/" << nextLevelExp << "\n";
        cout << "Weapon: ";
        weapon.display();
        cout << "Armor: ";
        armor.display();
        setColor(11);
        cout << "Strength: " << stats.at("strength") << " | Agility: " << stats.at("agility") << " | Intelligence: " << stats.at("intelligence") << "\n";
        setColor(15);
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= nextLevelExp) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience = 0;
        nextLevelExp = static_cast<int>(nextLevelExp * 1.5);
        maxHP += 20;
        HP = maxHP;
        stats["strength"] += 2;
        stats["agility"] += 2;
        stats["intelligence"] += 2;
        setColor(14);
        cout << "Level up! You are now level " << level << "!\n";
        setColor(15);
    }

    void attack(Enemy& enemy) {
        float damage = weapon.getDamage() + stats.at("strength");
        setColor(12);
        cout << "You attack " << enemy.name << " for " << damage << " damage!\n";
        setColor(15);
        enemy.HP -= damage;
        if (enemy.HP <= 0) {
            setColor(10);
            cout << "Enemy defeated! You gained " << enemy.experience << " experience.\n";
            setColor(15);
            gainExperience(enemy.experience);
        }
        else {
            enemy.attack(HP);
        }
    }

    void lootChest() {
        float lootGold = static_cast<float>(rand() % 100 + 1);
        gold += lootGold;

        int itemType = rand() % 3;
        switch(itemType) {
            case 0:
                inventory.push_back(Item("Health Potion", 20.0f, 1, common));
                break;
            case 1:
                inventory.push_back(Weapon("Random Sword", 30.0f, 80, uncommon, 12.0f));
                break;
            case 2:
                inventory.push_back(Armor("Random Armor", 40.0f, 80, uncommon, 8.0f));
                break;
        }

        setColor(14);
        cout << "You found " << lootGold << " gold and an item!\n";
        setColor(15);
    }

    void showInventoryGrid() const {
        system("cls");
        setColor(11);
        cout << "=== INVENTORY (4x4) ===\n";
        cout << "Gold: " << gold << "\n\n";

        cout << "    1   2   3   4  \n";
        cout << "  +---+---+---+---+\n";

        for (int row = 0; row < 4; ++row) {
            cout << row + 1 << " |";
            for (int col = 0; col < 4; ++col) {
                int index = row * 4 + col;
                if (index < inventory.size()) {
                    switch(inventory[index].getRarity()) {
                        case common: setColor(8); break;
                        case uncommon: setColor(2); break;
                        case magic: setColor(9); break;
                        case rare: setColor(5); break;
                        case legendary: setColor(6); break;
                    }
                    cout << " " << inventory[index].getName()[0] << " ";
                    setColor(15);
                    cout << "|";
                } else {
                    cout << "   |";
                }
            }
            cout << "\n  +---+---+---+---+\n";
        }
        setColor(15);
        cout << "\n[I] Inspect  [E] Equip  [X] Exit\n";
    }

    void inspectItem() const {
        if (inventory.empty()) {
            setColor(12);
            cout << "No items to inspect!\n";
            setColor(15);
            return;
        }

        int choice;
        do {
            showInventoryGrid();
            cout << "Select item to inspect (1-4 for column, 1-4 for row, or 0 to cancel): ";
            cin >> choice;

            if (choice == 0) return;

            int row = choice / 10 - 1;
            int col = choice % 10 - 1;

            if (row >= 0 && row < 4 && col >= 0 && col < 4) {
                int index = row * 4 + col;
                if (index < inventory.size()) {
                    system("cls");
                    setColor(14);
                    cout << "=== ITEM DETAILS ===\n";
                    inventory[index].display();
                    setColor(15);
                    cout << "\nPress any key to continue...";
                    _getch();
                }
            }
        } while (choice != 0);
    }

    void equipFromInventory() {
        if (inventory.empty()) {
            setColor(12);
            cout << "No items to equip!\n";
            setColor(15);
            return;
        }

        int choice;
        do {
            showInventoryGrid();
            cout << "Select item to equip (1-4 for column, 1-4 for row, or 0 to cancel): ";
            cin >> choice;

            if (choice == 0) return;

            int row = choice / 10 - 1;
            int col = choice % 10 - 1;

            if (row >= 0 && row < 4 && col >= 0 && col < 4) {
                int index = row * 4 + col;
                if (index < inventory.size()) {
                    Item& item = inventory[index];
                    if (dynamic_cast<Weapon*>(&item)) {
                        weapon = *dynamic_cast<Weapon*>(&item);
                        setColor(10);
                        cout << "Weapon equipped!\n";
                    }
                    else if (dynamic_cast<Armor*>(&item)) {
                        armor = *dynamic_cast<Armor*>(&item);
                        setColor(10);
                        cout << "Armor equipped!\n";
                    }
                    else {
                        setColor(12);
                        cout << "This item cannot be equipped!\n";
                    }
                    setColor(15);
                    cout << "Press any key to continue...";
                    _getch();
                }
            }
        } while (choice != 0);
    }

    void addItemToInventory(const Item& item) {
        if (inventory.size() < 16) {
            inventory.push_back(item);
            setColor(10);
            cout << "Item added to inventory!\n";
            setColor(15);
        } else {
            setColor(12);
            cout << "Inventory is full!\n";
            setColor(15);
        }
    }

    float getHP() const { return HP; }
    float getGold() const { return gold; }
    void spendGold(float amount) { gold -= amount; }
};

class NPC {
public:
    string name;
    vector<Item> shopItems;

    NPC(string n) : name(n) {
        shopItems.push_back(Item("Health Potion", 20.0f, 1, common));
        shopItems.push_back(Weapon("Steel Sword", 50.0f, 100, uncommon, 15.0f));
        shopItems.push_back(Armor("Chainmail", 75.0f, 100, uncommon, 10.0f));
    }

    void showShop() const {
        setColor(14);
        cout << "Welcome to " << name << "'s shop!\n";
        cout << "----------------------------\n";
        for (size_t i = 0; i < shopItems.size(); ++i) {
            cout << i + 1 << ". ";
            shopItems[i].display();
        }
        setColor(15);
    }

    void trade(Player& player) {
        char choice;
        do {
            system("cls");
            showShop();
            cout << "\nYour gold: " << player.getGold() << "\n";
            cout << "[1-" << shopItems.size() << "] Buy  [X] Exit\n";
            choice = _getch();

            if (choice >= '1' && choice <= '9') {
                int itemIndex = choice - '1';
                if (itemIndex < shopItems.size()) {
                    if (player.getGold() >= shopItems[itemIndex].getPrice()) {
                        player.addItemToInventory(shopItems[itemIndex]);
                        player.spendGold(shopItems[itemIndex].getPrice());
                        setColor(10);
                        cout << "Purchase successful!\n";
                        setColor(15);
                    } else {
                        setColor(12);
                        cout << "Not enough gold!\n";
                        setColor(15);
                    }
                    _getch();
                }
            }
        } while (choice != 'x' && choice != 'X');
    }
};

class Map {
private:
    vector<vector<char>> grid;
    int size;
    int playerX, playerY;
    vector<pair<int, int>> chests;
    vector<pair<int, int>> enemies;
    vector<pair<int, int>> npcs;
    vector<pair<int, int>> obstacles;
    Weather weather;
    string location;

public:
    Map(int s) : size(s), weather(sunny), location("Starting Area") {
        grid.resize(size, vector<char>(size, '.'));
        playerX = size / 2;
        playerY = size / 2;
        grid[playerY][playerX] = 'P';

        generateObstacles();
        generateChests();
        generateEnemies();
        generateNPCs();
    }

    void generateObstacles() {
        obstacles.clear();
        for (int i = 0; i < size; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.') {
                obstacles.push_back({ x, y });
                grid[y][x] = '#';
            }
        }
    }

    void generateChests() {
        chests.clear();
        for (int i = 0; i < 5; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.') {
                chests.push_back({ x, y });
                grid[y][x] = 'C';
            }
        }
    }

    void generateEnemies() {
        enemies.clear();
        for (int i = 0; i < 5; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.') {
                enemies.push_back({ x, y });
                grid[y][x] = 'E';
            }
        }
    }

    void generateNPCs() {
        npcs.clear();
        for (int i = 0; i < 2; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.') {
                npcs.push_back({ x, y });
                grid[y][x] = 'N';
            }
        }
    }

    void display() const {
        setCursorPosition(0, 0);
        setColor(11);
        cout << "Location: " << location << "\n";
        cout << "Weather: ";
        switch (weather) {
        case sunny: cout << "Sunny"; break;
        case rainy: cout << "Rainy"; break;
        case cloudy: cout << "Cloudy"; break;
        }
        cout << "\n";
        setColor(15);

        vector<vector<char>> displayGrid = grid;

        for (const auto& chest : chests) {
            displayGrid[chest.second][chest.first] = 'C';
        }
        for (const auto& enemy : enemies) {
            displayGrid[enemy.second][enemy.first] = 'E';
        }
        for (const auto& npc : npcs) {
            displayGrid[npc.second][npc.first] = 'N';
        }
        for (const auto& obstacle : obstacles) {
            displayGrid[obstacle.second][obstacle.first] = '#';
        }

        displayGrid[playerY][playerX] = 'P';

        for (auto& row : displayGrid) {
            for (char tile : row) {
                if (tile == 'P') setColor(10);
                else if (tile == 'C') setColor(14);
                else if (tile == 'E') setColor(12);
                else if (tile == 'N') setColor(11);
                else if (tile == '#') setColor(8);
                else setColor(15);
                cout << tile << " ";
            }
            cout << endl;
        }
        setColor(15);
    }

    void movePlayer(char direction) {
        int newX = playerX;
        int newY = playerY;

        switch (direction) {
        case 'w': case 'W': newY--; break;
        case 's': case 'S': newY++; break;
        case 'a': case 'A': newX--; break;
        case 'd': case 'D': newX++; break;
        }

        if (newX >= 0 && newX < size && newY >= 0 && newY < size && grid[newY][newX] != '#') {
            grid[playerY][playerX] = '.';
            playerX = newX;
            playerY = newY;
            grid[playerY][playerX] = 'P';
        }

        if (playerX == 0 || playerX == size - 1 || playerY == 0 || playerY == size - 1) {
            generateNewTerrain();
        }
    }

    void generateNewTerrain() {
        location = "New Area";
        generateObstacles();
        generateChests();
        generateEnemies();
        generateNPCs();
    }

    bool isNearChest(int& chestX, int& chestY) const {
        for (auto& chest : chests) {
            if (abs(chest.first - playerX) <= 1 && abs(chest.second - playerY) <= 1) {
                chestX = chest.first;
                chestY = chest.second;
                return true;
            }
        }
        return false;
    }

    bool isNearEnemy(int& enemyX, int& enemyY) const {
        for (auto& enemy : enemies) {
            if (abs(enemy.first - playerX) <= 1 && abs(enemy.second - playerY) <= 1) {
                enemyX = enemy.first;
                enemyY = enemy.second;
                return true;
            }
        }
        return false;
    }

    bool isNearNPC(int& npcX, int& npcY) const {
        for (auto& npc : npcs) {
            if (abs(npc.first - playerX) <= 1 && abs(npc.second - playerY) <= 1) {
                npcX = npc.first;
                npcY = npc.second;
                return true;
            }
        }
        return false;
    }

    void removeChest(int x, int y) {
        for (auto it = chests.begin(); it != chests.end(); ++it) {
            if (it->first == x && it->second == y) {
                chests.erase(it);
                grid[y][x] = '.';
                break;
            }
        }
    }

    void removeEnemy(int x, int y) {
        for (auto it = enemies.begin(); it != enemies.end(); ++it) {
            if (it->first == x && it->second == y) {
                enemies.erase(it);
                grid[y][x] = '.';
                break;
            }
        }
    }

    void removeNPC(int x, int y) {
        for (auto it = npcs.begin(); it != npcs.end(); ++it) {
            if (it->first == x && it->second == y) {
                npcs.erase(it);
                grid[y][x] = '.';
                break;
            }
        }
    }

    void changeWeather() {
        weather = static_cast<Weather>((rand() % 3));
    }
};

int main() {
    srand(time(0));
    Map gameMap(10);
    Player player("Hero");
    NPC npc("Merchant");

    hideCursor();

    while (player.getHP() > 0) {
        gameMap.display();
        player.showStats();
        cout << "Move (W/A/S/D), fight (F), loot (L), trade (T), inventory (I): ";

        if (_kbhit()) {
            char input = _getch();
            int x, y;

            if (input == 'f' || input == 'F') {
                if (gameMap.isNearEnemy(x, y)) {
                    Enemy enemy("Goblin", 30.0f, 5.0f, 50);
                    player.attack(enemy);
                    if (enemy.HP <= 0) {
                        gameMap.removeEnemy(x, y);
                    }
                }
                else {
                    setColor(12);
                    cout << "No enemy nearby!\n";
                    setColor(15);
                }
            }
            else if (input == 'l' || input == 'L') {
                if (gameMap.isNearChest(x, y)) {
                    player.lootChest();
                    gameMap.removeChest(x, y);
                }
                else {
                    setColor(12);
                    cout << "No chest nearby!\n";
                    setColor(15);
                }
            }
            else if (input == 't' || input == 'T') {
                if (gameMap.isNearNPC(x, y)) {
                    npc.trade(player);
                }
                else {
                    setColor(12);
                    cout << "No NPC nearby!\n";
                    setColor(15);
                }
            }
            else if (input == 'i' || input == 'I') {
                char inventoryChoice;
                do {
                    player.showInventoryGrid();
                    inventoryChoice = _getch();

                    switch(tolower(inventoryChoice)) {
                        case 'i':
                            player.inspectItem();
                            break;
                        case 'e':
                            player.equipFromInventory();
                            break;
                    }
                } while (tolower(inventoryChoice) != 'x');
            }
            else {
                gameMap.movePlayer(input);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    setColor(12);
    cout << "Game Over!" << endl;
    setColor(15);
    return 0;
}
