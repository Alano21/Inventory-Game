#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <map>
#include <windows.h> // Do kolorów i podwójnego buforowania
#include <conio.h>   // Do funkcji _kbhit() i _getch()
using namespace std;

// Funkcja do zmiany koloru tekstu w terminalu
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Funkcja do ustawienia pozycji kursora w konsoli
void setCursorPosition(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Funkcja do ukrycia kursora
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false; // Ukryj kursor
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
        setColor(15); // Bia³y tekst
        cout << name << " (" << price << "g, Durability: " << durability << ", Rarity: " << rarity << ")\n";
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
        setColor(12); // Czerwony tekst
        cout << "Damage: " << damage << "\n";
        setColor(15); // Powrót do bia³ego tekstu
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
        setColor(11); // Jasnoniebieski tekst
        cout << "Defense: " << defense << "\n";
        setColor(15); // Powrót do bia³ego tekstu
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
        setColor(12); // Czerwony tekst
        cout << name << " attacks! Player HP: " << playerHP << "\n";
        setColor(15); // Powrót do bia³ego tekstu
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
    Player(string n) : name(n), HP(100.0f), maxHP(100.0f), gold(50.0f), level(1), experience(0), nextLevelExp(100) {
        stats["strength"] = 10;
        stats["agility"] = 10;
        stats["intelligence"] = 10;
    }

    void showStats() const {
        setColor(10); // Zielony tekst
        cout << "Player: " << name << " | HP: " << HP << "/" << maxHP << " | Gold: " << gold << "\n";
        setColor(14); // ¯ó³ty tekst
        cout << "Level: " << level << " | Exp: " << experience << "/" << nextLevelExp << "\n";
        weapon.display();
        armor.display();
        setColor(11); // Jasnoniebieski tekst
        cout << "Strength: " << stats.at("strength") << " | Agility: " << stats.at("agility") << " | Intelligence: " << stats.at("intelligence") << "\n";
        setColor(15); // Powrót do bia³ego tekstu
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
        setColor(14); // ¯ó³ty tekst
        cout << "Level up! You are now level " << level << "!\n";
        setColor(15); // Powrót do bia³ego tekstu
    }

    void attack(Enemy& enemy) {
        float damage = weapon.getDamage() + stats.at("strength");
        setColor(12); // Czerwony tekst
        cout << "You attack " << enemy.name << " for " << damage << " damage!\n";
        setColor(15); // Powrót do bia³ego tekstu
        enemy.HP -= damage;
        if (enemy.HP <= 0) {
            setColor(10); // Zielony tekst
            cout << "Enemy defeated! You gained " << enemy.experience << " experience.\n";
            setColor(15); // Powrót do bia³ego tekstu
            gainExperience(enemy.experience);
        }
        else {
            enemy.attack(HP);
        }
    }

    void lootChest() {
        float lootGold = static_cast<float>(rand() % 100 + 1);
        gold += lootGold;
        setColor(14); // ¯ó³ty tekst
        cout << "You found " << lootGold << " gold!\n";
        setColor(15); // Powrót do bia³ego tekstu
    }

    void showInventory() const {
        setColor(11); // Jasnoniebieski tekst
        cout << "Inventory:\n";
        for (const auto& item : inventory) {
            item.display();
        }
        setColor(15); // Powrót do bia³ego tekstu
    }

    void equipItem() {
        if (inventory.empty()) {
            setColor(12); // Czerwony tekst
            cout << "No items to equip!\n";
            setColor(15); // Powrót do bia³ego tekstu
            return;
        }

        showInventory();
        cout << "Select item to equip (1-" << inventory.size() << "): ";
        int choice;
        cin >> choice;

        if (choice > 0 && choice <= inventory.size()) {
            Item& item = inventory[choice - 1];
            if (dynamic_cast<Weapon*>(&item)) {
                weapon = *dynamic_cast<Weapon*>(&item);
                setColor(10); // Zielony tekst
                cout << "Weapon equipped!\n";
            }
            else if (dynamic_cast<Armor*>(&item)) {
                armor = *dynamic_cast<Armor*>(&item);
                setColor(10); // Zielony tekst
                cout << "Armor equipped!\n";
            }
            setColor(15); // Powrót do bia³ego tekstu
        }
    }

    void addItemToInventory(const Item& item) {
        if (inventory.size() < 9) {
            inventory.push_back(item);
            setColor(10); // Zielony tekst
            cout << "Item added to inventory!\n";
            setColor(15); // Powrót do bia³ego tekstu
        }
        else {
            setColor(12); // Czerwony tekst
            cout << "Inventory is full!\n";
            setColor(15); // Powrót do bia³ego tekstu
        }
    }

    void removeItemFromInventory() {
        if (inventory.empty()) {
            setColor(12); // Czerwony tekst
            cout << "No items to remove!\n";
            setColor(15); // Powrót do bia³ego tekstu
            return;
        }

        showInventory();
        cout << "Select item to remove (1-" << inventory.size() << "): ";
        int choice;
        cin >> choice;

        if (choice > 0 && choice <= inventory.size()) {
            inventory.erase(inventory.begin() + choice - 1);
            setColor(10); // Zielony tekst
            cout << "Item removed from inventory!\n";
            setColor(15); // Powrót do bia³ego tekstu
        }
    }

    float getHP() const { return HP; }
};

class NPC {
public:
    string name;
    vector<Item> shopItems;

    NPC(string n) : name(n) {
        shopItems.push_back(Item("Health Potion", 20.0f, 1, common));
        shopItems.push_back(Item("Steel Sword", 50.0f, 100, uncommon));
    }

    void showShop() const {
        setColor(14); // ¯ó³ty tekst
        cout << "Welcome to " << name << "'s shop!\n";
        for (size_t i = 0; i < shopItems.size(); ++i) {
            cout << i + 1 << ". ";
            shopItems[i].display();
        }
        setColor(15); // Powrót do bia³ego tekstu
    }

    void trade(Player& player) const {
        showShop();
        cout << "What do you want to buy? (Enter number or 0 to exit): ";
        int choice;
        cin >> choice;
        if (choice > 0 && choice <= shopItems.size()) {
            Item item = shopItems[choice - 1];
            player.addItemToInventory(item);
        }
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
        setCursorPosition(0, 0); // Ustaw kursor na pocz¹tek konsoli
        setColor(11); // Jasnoniebieski tekst
        cout << "Location: " << location << "\n";
        cout << "Weather: ";
        switch (weather) {
        case sunny: cout << "Sunny"; break;
        case rainy: cout << "Rainy"; break;
        case cloudy: cout << "Cloudy"; break;
        }
        cout << "\n";
        setColor(15); // Powrót do bia³ego tekstu

        for (auto& row : grid) {
            for (char tile : row) {
                if (tile == 'P') setColor(10); // Zielony tekst (gracz)
                else if (tile == 'C') setColor(14); // ¯ó³ty tekst (skrzynia)
                else if (tile == 'E') setColor(12); // Czerwony tekst (przeciwnik)
                else if (tile == 'N') setColor(11); // Jasnoniebieski tekst (NPC)
                else if (tile == '#') setColor(8); // Szary tekst (przeszkoda)
                else setColor(15); // Bia³y tekst (reszta)
                cout << tile << " ";
            }
            cout << endl;
        }
        setColor(15); // Powrót do bia³ego tekstu
    }

    void movePlayer(char direction) {
        grid[playerY][playerX] = '.';
        switch (direction) {
        case 'w': case 'W': if (playerY > 0 && grid[playerY - 1][playerX] != '#') playerY--; break;
        case 's': case 'S': if (playerY < size - 1 && grid[playerY + 1][playerX] != '#') playerY++; break;
        case 'a': case 'A': if (playerX > 0 && grid[playerY][playerX - 1] != '#') playerX--; break;
        case 'd': case 'D': if (playerX < size - 1 && grid[playerY][playerX + 1] != '#') playerX++; break;
        }
        grid[playerY][playerX] = 'P';

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
    Enemy enemy("Goblin", 30.0f, 5.0f, 50);
    NPC npc("Merchant");

    hideCursor(); // Ukryj kursor

    while (player.getHP() > 0) {
        gameMap.display();
        player.showStats();
        cout << "Move (W/A/S/D), fight (F), loot (L), trade (T), inventory (I): ";

        if (_kbhit()) {
            char input = _getch();
            int x, y;

            if (input == 'f' || input == 'F') {
                if (gameMap.isNearEnemy(x, y)) {
                    player.attack(enemy);
                    if (enemy.HP <= 0) {
                        gameMap.removeEnemy(x, y); // Usuñ przeciwnika po pokonaniu
                    }
                }
                else {
                    setColor(12); // Czerwony tekst
                    cout << "No enemy nearby!\n";
                    setColor(15); // Powrót do bia³ego tekstu
                }
            }
            else if (input == 'l' || input == 'L') {
                if (gameMap.isNearChest(x, y)) {
                    player.lootChest();
                    gameMap.removeChest(x, y); // Usuñ skrzyniê po otwarciu
                }
                else {
                    setColor(12); // Czerwony tekst
                    cout << "No chest nearby!\n";
                    setColor(15); // Powrót do bia³ego tekstu
                }
            }
            else if (input == 't' || input == 'T') {
                if (gameMap.isNearNPC(x, y)) {
                    npc.trade(player);
                    gameMap.removeNPC(x, y); // Usuñ NPC po interakcji
                }
                else {
                    setColor(12); // Czerwony tekst
                    cout << "No NPC nearby!\n";
                    setColor(15); // Powrót do bia³ego tekstu
                }
            }
            else if (input == 'i' || input == 'I') {
                player.showInventory();
                cout << "Equip item (E), remove item (R): ";
                char inventoryInput = _getch();
                if (inventoryInput == 'e' || inventoryInput == 'E') {
                    player.equipItem();
                }
                else if (inventoryInput == 'r' || inventoryInput == 'R') {
                    player.removeItemFromInventory();
                }
            }
            else {
                gameMap.movePlayer(input);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    setColor(12); // Czerwony tekst
    cout << "Game Over!" << endl;
    setColor(15); // Powrót do bia³ego tekstu
    return 0;
}
