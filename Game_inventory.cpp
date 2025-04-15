#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <map>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

// Definicja typów wyliczeniowych dla rzadkości przedmiotów i pogody
enum Rarities { common, uncommon, magic, rare, legendary };
enum Weather { sunny, rainy, cloudy };

// Klasa bazowa dla przedmiotów w grze
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
        cout << "\033[37m";
        cout << name << " (" << price << "g, Durability: " << durability << ", Rarity: ";
        switch(rarity) {
        case common: cout << "\033[90mCommon"; break;
        case uncommon: cout << "\033[32mUncommon"; break;
        case magic: cout << "\033[94mMagic"; break;
        case rare: cout << "\033[35mRare"; break;
        case legendary: cout << "\033[36mLegendary"; break;
        }
        cout << "\033[37m";
        cout << ")\n";
    }

    string getName() const { return name; }
    float getPrice() const { return price; }
    int getDurability() const { return durability; }
    Rarities getRarity() const { return rarity; }

    void setPrice(float newPrice) { price = newPrice; }
    void setDurability(int newDurability) { durability = newDurability; }
};

// Klasa dla broni
class Weapon : public Item {
private:
    float damage;
public:
    Weapon() : Item("Basic Sword", 10.0f, 100, common), damage(10.0f) {}
    Weapon(string name, float price, int durability, Rarities rarity, float damage)
        : Item(name, price, durability, rarity), damage(damage) {}

    void display() const override {
        Item::display();
        cout << "\033[93m";
        cout << "Damage: " << damage << "\n";
        cout << "\033[37m";
    }

    float getDamage() const { return damage; }
};

// Klasa dla zbroi
class Armor : public Item {
private:
    float defense;
public:
    Armor() : Item("Basic Armor", 20.0f, 100, common), defense(5.0f) {}
    Armor(string name, float price, int durability, Rarities rarity, float defense)
        : Item(name, price, durability, rarity), defense(defense) {}

    void display() const override {
        Item::display();
        cout << "\033[92m";
        cout << "Defense: " << defense << "\n";
        cout << "\033[37m";
    }

    float getDefense() const { return defense; }
};

// Klasa dla przeciwników
class Enemy {
public:
    string name;
    float HP;
    float damage;
    int experience;

    Enemy(string n, float hp, float dmg) : name(n), HP(hp), damage(dmg), experience(20) {}

    void attack(float& playerHP) const {
        playerHP -= damage;
        cout << "\033[93m";
        cout << name << " attacks! Player HP: " << playerHP << "\n";
        cout << "\033[37m";
    }
};

// Klasa dla gracza
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
    vector<Item*> inventory;
    map<string, int> stats;

public:
    Player(string n) : name(n), HP(100.0f), maxHP(100.0f), gold(100.0f), level(1), experience(0), nextLevelExp(100) {
        stats["strength"] = 10;
        stats["agility"] = 10;
        stats["intelligence"] = 10;

        inventory.push_back(new Weapon("Basic Sword", 10.0f, 100, common, 10.0f));
        inventory.push_back(new Armor("Basic Armor", 20.0f, 100, common, 5.0f));
    }

    ~Player() {
        for (auto item : inventory) {
            delete item;
        }
    }

    void showStats() const {
        cout << "\033[91m";
        cout << "Player: " << name << " | HP: " << HP << "/" << maxHP << " | Gold: " << gold << "\n";
        cout << "\033[96m";
        cout << "Level: " << level << " | Exp: " << experience << "/" << nextLevelExp << "\n";
        cout << "Weapon: ";
        weapon.display();
        cout << "Armor: ";
        armor.display();
        cout << "\033[92m";
        cout << "Strength: " << stats.at("strength") << " | Agility: " << stats.at("agility") << " | Intelligence: " << stats.at("intelligence") << "\n";
        cout << "\033[37m";
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
        cout << "\033[96m";
        cout << "Level Up! You are now level " << level << "!\n";
        cout << "\033[37m";
    }

    void attack(Enemy& enemy) {
        float damage = weapon.getDamage() + stats.at("strength");
        cout << "\033[93m";
        cout << "You attack " << enemy.name << " for " << damage << " damage!\n";
        cout << "\033[37m";
        enemy.HP -= damage;
        if (enemy.HP <= 0) {
            cout << "\033[91m";
            cout << "Enemy defeated! Gained " << enemy.experience << " experience.\n";
            cout << "\033[37m";
            gainExperience(enemy.experience);
        } else {
            enemy.attack(HP);
            this_thread::sleep_for(chrono::seconds(3));
            cout << "\033[2J\033[1;1H"; // Clear screen after 3 seconds
        }
    }

    void lootChest() {
        float lootGold = static_cast<float>(rand() % 100 + 1);
        gold += lootGold;
        int expGained = 20; // EXP for looting a chest
        gainExperience(expGained);

        int itemType = rand() % 3;
        switch(itemType) {
        case 0:
            inventory.push_back(new Item("Health Potion", 20.0f, 1, common));
            break;
        case 1:
            inventory.push_back(new Weapon("Random Sword", 30.0f, 80, uncommon, 12.0f));
            break;
        case 2:
            inventory.push_back(new Armor("Random Armor", 40.0f, 80, uncommon, 8.0f));
            break;
        }

        cout << "\033[96m";
        cout << "Found " << lootGold << " gold, an item, and " << expGained << " EXP!\n";
        cout << "\033[37m";
        this_thread::sleep_for(chrono::seconds(3)); // Wait 3 seconds
        cout << "\033[2J\033[1;1H"; // Clear screen
    }

    void showInventoryGridWithCursor(int cursorRow, int cursorCol) const {
        cout << "\033[2J\033[1;1H";
        cout << "\033[92m";
        cout << "=== INVENTORY (4x4) ===\n";
        cout << "Gold: " << gold << "\n\n";

        cout << "    1   2   3   4  \n";
        cout << "  +---+---+---+---+\n";

        for (int row = 0; row < 4; ++row) {
            cout << row + 1 << " |";
            for (int col = 0; col < 4; ++col) {
                int index = row * 4 + col;
                if (row == cursorRow && col == cursorCol) {
                    cout << "\033[91m";
                    cout << "[";
                } else {
                    cout << " ";
                }
                if (index < inventory.size()) {
                    switch(inventory[index]->getRarity()) {
                    case common: cout << "\033[90m"; break;
                    case uncommon: cout << "\033[32m"; break;
                    case magic: cout << "\033[94m"; break;
                    case rare: cout << "\033[35m"; break;
                    case legendary: cout << "\033[36m"; break;
                    }
                    cout << inventory[index]->getName()[0];
                    cout << "\033[37m";
                } else {
                    cout << " ";
                }
                if (row == cursorRow && col == cursorCol) {
                    cout << "\033[91m";
                    cout << "]";
                } else {
                    cout << " ";
                }
                cout << "\033[37m";
                cout << "|";
            }
            cout << "\n  +---+---+---+---+\n";
        }
        cout << "\033[37m";
        cout << "\n[I] Inspect  [E] Equip  [X] Exit\n";
    }

    void inspectItem() const {
        if (inventory.empty()) {
            cout << "\033[93m";
            cout << "No items to inspect!\n";
            cout << "\033[37m";
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;

        do {
            showInventoryGridWithCursor(cursorRow, cursorCol);
            input = tolower(getch());

            if (input == 'w' && cursorRow > 0) cursorRow--;
            if (input == 's' && cursorRow < 3) cursorRow++;
            if (input == 'a' && cursorCol > 0) cursorCol--;
            if (input == 'd' && cursorCol < 3) cursorCol++;

            if (input == 'i') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    cout << "\033[2J\033[1;1H";
                    cout << "\033[96m";
                    cout << "=== ITEM DETAILS ===\n";
                    inventory[index]->display();
                    cout << "\033[37m";
                    cout << "\nPress any key to continue...";
                    getch();
                }
            }
        } while (input != 'x');
    }

    void equipFromInventory() {
        if (inventory.empty()) {
            cout << "\033[93m";
            cout << "No items to equip!\n";
            cout << "\033[37m";
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;

        do {
            showInventoryGridWithCursor(cursorRow, cursorCol);
            input = tolower(getch());

            if (input == 'w' && cursorRow > 0) cursorRow--;
            if (input == 's' && cursorRow < 3) cursorRow++;
            if (input == 'a' && cursorCol > 0) cursorCol--;
            if (input == 'd' && cursorCol < 3) cursorCol++;

            if (input == 'e') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    Item* item = inventory[index];
                    if (Weapon* weaponPtr = dynamic_cast<Weapon*>(item)) {
                        weapon = *weaponPtr;
                        cout << "\033[91m";
                        cout << "Weapon equipped!\n";
                    } else if (Armor* armorPtr = dynamic_cast<Armor*>(item)) {
                        armor = *armorPtr;
                        cout << "\033[91m";
                        cout << "Armor equipped!\n";
                    } else {
                        cout << "\033[93m";
                        cout << "This item cannot be equipped!\n";
                    }
                    cout << "\033[37m";
                    cout << "Press any key to continue...";
                    getch();
                }
            }
        } while (input != 'x');
    }

    void addItemToInventory(const Item& item) {
        if (inventory.size() < 16) {
            if (dynamic_cast<const Weapon*>(&item)) {
                inventory.push_back(new Weapon(dynamic_cast<const Weapon&>(item)));
            } else if (dynamic_cast<const Armor*>(&item)) {
                inventory.push_back(new Armor(dynamic_cast<const Armor&>(item)));
            } else {
                inventory.push_back(new Item(item));
            }
            cout << "\033[91m";
            cout << "Item added to inventory!\n";
            cout << "\033[37m";
        } else {
            cout << "\033[93m";
            cout << "Inventory full!\n";
            cout << "\033[37m";
        }
    }

    float getHP() const { return HP; }
    float getGold() const { return gold; }
    int getLevel() const { return level; }

    void spendGold(float amount) { gold -= amount; }

private:
    static int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
};

// Klasa dla NPC
class NPC {
public:
    string name;
    vector<Item*> shopItems;

    NPC(string n) : name(n) {
        shopItems.push_back(new Item("Health Potion", 20.0f, 1, common));
        shopItems.push_back(new Weapon("Steel Sword", 50.0f, 100, uncommon, 15.0f));
        shopItems.push_back(new Armor("Chainmail", 75.0f, 100, uncommon, 10.0f));
    }

    ~NPC() {
        for (auto item : shopItems) {
            delete item;
        }
    }

    void showShop() const {
        cout << "\033[96m";
        cout << "Welcome to " << name << "'s Shop!\n";
        cout << "----------------------------\n";
        for (size_t i = 0; i < shopItems.size(); ++i) {
            cout << i + 1 << ". ";
            shopItems[i]->display();
        }
        cout << "\033[37m";
    }

    void trade(Player& player) {
        char choice;
        do {
            cout << "\033[2J\033[1;1H";
            showShop();
            cout << "\nYour gold: " << player.getGold() << "\n";
            cout << "[1-" << shopItems.size() << "] Buy  [X] Exit\n";
            choice = getch();

            if (choice >= '1' && choice <= '9') {
                int itemIndex = choice - '1';
                if (itemIndex < shopItems.size()) {
                    if (player.getGold() >= shopItems[itemIndex]->getPrice()) {
                        player.addItemToInventory(*shopItems[itemIndex]);
                        player.spendGold(shopItems[itemIndex]->getPrice());
                        cout << "\033[91m";
                        cout << "Purchase successful!\n";
                        cout << "\033[37m";
                    } else {
                        cout << "\033[93m";
                        cout << "Not enough gold!\n";
                        cout << "\033[37m";
                    }
                    getch();
                }
            }
        } while (choice != 'x' && choice != 'X');
        cout << "\033[2J\033[1;1H";
    }

private:
    static int getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
};

// Klasa dla mapy
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
    Map(int s, int level) : size(s), weather(sunny), location("Map Level " + to_string(level)) {
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
        int numObstacles = rand() % size + 5;
        for (int i = 0; i < numObstacles; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
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
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
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
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
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
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                npcs.push_back({ x, y });
                grid[y][x] = 'N';
            }
        }
    }

    void display() const {
        printf("\033[1;1H");
        cout << "\033[92m";
        cout << "Location: " << location << "\n";
        cout << "Weather: ";
        switch (weather) {
        case sunny: cout << "Sunny"; break;
        case rainy: cout << "Rainy"; break;
        case cloudy: cout << "Cloudy"; break;
        }
        cout << "\n";
        cout << "\033[37m";

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
                if (tile == 'P') cout << "\033[91m";
                else if (tile == 'C') cout << "\033[96m";
                else if (tile == 'E') cout << "\033[93m";
                else if (tile == 'N') cout << "\033[92m";
                else if (tile == '#') cout << "\033[90m";
                else cout << "\033[37m";
                cout << tile << " ";
            }
            cout << endl;
        }
        cout << "\033[37m";
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

    void changeWeather() {
        weather = static_cast<Weather>(rand() % 3);
    }
};

// Funkcja do sprawdzania naciśnięcia klawisza
static bool kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}

// Funkcja do odczytu pojedynczego znaku
static int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Główna funkcja gry
int main() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    srand(time(0));
    Player player("Hero");
    NPC npc("Merchant");
    Map gameMap(10, player.getLevel());
    int currentLevel = player.getLevel();

    while (player.getHP() > 0) {
        if (player.getLevel() > currentLevel) {
            gameMap = Map(10, player.getLevel());
            currentLevel = player.getLevel();
            cout << "\033[96m";
            cout << "New map generated for level " << currentLevel << "!\n";
            cout << "\033[37m";
            this_thread::sleep_for(chrono::seconds(2));
        }

        gameMap.display();
        player.showStats();
        cout << "Move (W/A/S/D), Fight (F), Loot (L), Trade (T), Inventory (I): ";
        cout.flush();

        if (kbhit()) {
            char input = getch();
            int x, y;

            if (input == 'f' || input == 'F') {
                if (gameMap.isNearEnemy(x, y)) {
                    Enemy enemy("Goblin", 30.0f, 5.0f);
                    player.attack(enemy);
                    if (enemy.HP <= 0) {
                        gameMap.removeEnemy(x, y);
                    }
                } else {
                    cout << "\033[93m";
                    cout << "No enemy nearby!\n";
                    cout << "\033[37m";
                }
            } else if (input == 'l' || input == 'L') {
                if (gameMap.isNearChest(x, y)) {
                    player.lootChest();
                    gameMap.removeChest(x, y);
                } else {
                    cout << "\033[93m";
                    cout << "No chest nearby!\n";
                    cout << "\033[37m";
                }
            } else if (input == 't' || input == 'T') {
                if (gameMap.isNearNPC(x, y)) {
                    npc.trade(player);
                } else {
                    cout << "\033[93m";
                    cout << "No NPC nearby!\n";
                    cout << "\033[37m";
                }
            } else if (input == 'i' || input == 'I') {
                char inventoryChoice;
                do {
                    player.showInventoryGridWithCursor(0, 0);
                    inventoryChoice = getch();

                    switch(tolower(inventoryChoice)) {
                    case 'i':
                        player.inspectItem();
                        break;
                    case 'e':
                        player.equipFromInventory();
                        break;
                    }
                } while (tolower(inventoryChoice) != 'x');
            } else {
                gameMap.movePlayer(input);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    cout << "\033[93m";
    cout << "Game Over!" << endl;
    cout << "\033[37m";
    return 0;
}
