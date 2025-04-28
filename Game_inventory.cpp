#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <map>
#include <functional>
#include <iomanip>
#include <random>
#include <fstream>
#include <filesystem>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
using namespace std;
namespace fs = std::filesystem;

// Wyliczenia dla rzadkości przedmiotów i pogody
enum Rarities { common, uncommon, magic, rare, legendary };
enum Weather { sunny, rainy, cloudy };

// Obsługa wejścia w systemach Windows i Unix
#ifdef _WIN32
int getch() { return _getch(); }
int kbhit() { return _kbhit(); }
#else
int getch() {
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

bool kbhit() {
    struct termios oldt, newt;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}
#endif

// Forward declaration of Player
class Player;

// Bazowa klasa dla przedmiotów
class Item {
protected:
    string name;
    float price;
    int durability;
    Rarities rarity;
    bool isFavorite;
public:
    Item() : name("Scrap"), price(0.1f), durability(100), rarity(common), isFavorite(false) {}
    Item(string name, float price, int durability, Rarities rarity)
        : name(name), price(price), durability(durability), rarity(rarity), isFavorite(false) {}
    virtual ~Item() {}

    virtual void display() const {
        cout << "\033[37m";
        cout << name << " (" << price << "g, Durability: " << durability << ", Rarity: ";
        switch (rarity) {
        case common: cout << "\033[90mCommon"; break;
        case uncommon: cout << "\033[32mUncommon"; break;
        case magic: cout << "\033[34mMagic"; break;
        case rare: cout << "\033[35mRare"; break;
        case legendary: cout << "\033[33mLegendary"; break;
        }
        cout << "\033[37m";
        cout << ", Favorite: " << (isFavorite ? "Yes" : "No") << ")\n";
    }

    virtual void use(Player& player) {
        cout << "\033[31mThis item cannot be used directly!\n\033[37m";
    }

    string getName() const { return name; }
    float getPrice() const { return price; }
    int getDurability() const { return durability; }
    Rarities getRarity() const { return rarity; }
    bool getIsFavorite() const { return isFavorite; }

    void setPrice(float newPrice) { price = newPrice; }
    void setDurability(int newDurability) { durability = newDurability; }

    void toggleFavorite() {
        isFavorite = !isFavorite;
        if (isFavorite) {
            if (name.find("#favorite") == string::npos) {
                name += "#favorite";
            }
        } else {
            size_t pos = name.find("#favorite");
            if (pos != string::npos) {
                name.erase(pos, string("#favorite").length());
            }
        }
    }

    virtual string serialize() const {
        stringstream ss;
        ss << "Item|" << name << "|" << price << "|" << durability << "|" << rarity << "|" << isFavorite;
        return ss.str();
    }

    static Item* deserialize(const string& data);
};

// Klasa dla mikstury zdrowia
class HealthPotion : public Item {
private:
    float healAmount;
public:
    HealthPotion() : Item("Health Potion", 20.0f, 1, common), healAmount(30.0f) {}

    void use(Player& player) override; // Declare here, define after Player

    void display() const override {
        cout << left << setw(20) << name;
        switch (rarity) {
        case common: cout << "\033[90m"; break;
        case uncommon: cout << "\033[32m"; break;
        case magic: cout << "\033[34m"; break;
        case rare: cout << "\033[35m"; break;
        case legendary: cout << "\033[33m"; break;
        }
        cout << setw(12) << getRarityString();
        cout << "\033[37m" << setw(8) << fixed << setprecision(1) << price;
        cout << setw(20) << "Heal: " + to_string((int)healAmount) + " HP";
        cout << "\033[37m";
    }

    string serialize() const override {
        stringstream ss;
        ss << "HealthPotion|" << name << "|" << price << "|" << durability << "|" << rarity << "|" << isFavorite << "|" << healAmount;
        return ss.str();
    }

private:
    string getRarityString() const {
        switch (rarity) {
        case common: return "Common";
        case uncommon: return "Uncommon";
        case magic: return "Magic";
        case rare: return "Rare";
        case legendary: return "Legendary";
        default: return "Unknown";
        }
    }
};

// Klasa dla broni
class Weapon : public Item {
private:
    float damage;
    int strengthBonus;
public:
    Weapon() : Item("Basic Sword", 50.0f, 100, common), damage(20.0f), strengthBonus(2) {}
    Weapon(string name, float price, int durability, Rarities rarity, float damage, int strengthBonus)
        : Item(name, price, durability, rarity), damage(damage), strengthBonus(strengthBonus) {}

    void display() const override {
        cout << left << setw(20) << name;
        switch (rarity) {
        case common: cout << "\033[90m"; break;
        case uncommon: cout << "\033[32m"; break;
        case magic: cout << "\033[34m"; break;
        case rare: cout << "\033[35m"; break;
        case legendary: cout << "\033[33m"; break;
        }
        cout << setw(12) << getRarityString();
        cout << "\033[37m" << setw(8) << fixed << setprecision(1) << price;
        cout << setw(20) << "Damage: " + to_string((int)damage) + ", Strength Bonus: +" + to_string(strengthBonus);
        cout << "\033[37m";
    }

    float getDamage() const { return damage; }
    int getStrengthBonus() const { return strengthBonus; }

    string serialize() const override {
        stringstream ss;
        ss << "Weapon|" << name << "|" << price << "|" << durability << "|" << rarity << "|" << isFavorite << "|" << damage << "|" << strengthBonus;
        return ss.str();
    }

private:
    string getRarityString() const {
        switch (rarity) {
        case common: return "Common";
        case uncommon: return "Uncommon";
        case magic: return "Magic";
        case rare: return "Rare";
        case legendary: return "Legendary";
        default: return "Unknown";
        }
    }
};

// Klasa dla zbroi
class Armor : public Item {
private:
    float defense;
    int agilityBonus;
public:
    Armor() : Item("Basic Armor", 60.0f, 100, common), defense(5.0f), agilityBonus(2) {}
    Armor(string name, float price, int durability, Rarities rarity, float defense, int agilityBonus)
        : Item(name, price, durability, rarity), defense(defense), agilityBonus(agilityBonus) {}

    void display() const override {
        cout << left << setw(20) << name;
        switch (rarity) {
        case common: cout << "\033[90m"; break;
        case uncommon: cout << "\033[32m"; break;
        case magic: cout << "\033[34m"; break;
        case rare: cout << "\033[35m"; break;
        case legendary: cout << "\033[33m"; break;
        }
        cout << setw(12) << getRarityString();
        cout << "\033[37m" << setw(8) << fixed << setprecision(1) << price;
        cout << setw(20) << "Defense: " + to_string((int)defense) + ", Agility Bonus: +" + to_string(agilityBonus);
        cout << "\033[37m";
    }

    float getDefense() const { return defense; }
    int getAgilityBonus() const { return agilityBonus; }

    string serialize() const override {
        stringstream ss;
        ss << "Armor|" << name << "|" << price << "|" << durability << "|" << rarity << "|" << isFavorite << "|" << defense << "|" << agilityBonus;
        return ss.str();
    }

private:
    string getRarityString() const {
        switch (rarity) {
        case common: return "Common";
        case uncommon: return "Uncommon";
        case magic: return "Magic";
        case rare: return "Rare";
        case legendary: return "Legendary";
        default: return "Unknown";
        }
    }
};

// Deserializacja przedmiotu
Item* Item::deserialize(const string& data) {
    vector<string> tokens;
    stringstream ss(data);
    string token;
    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.empty()) return nullptr;

    string type = tokens[0];
    Item* item = nullptr;

    if (type == "Item") {
        if (tokens.size() != 6) return nullptr;
        string name = tokens[1];
        float price = stof(tokens[2]);
        int durability = stoi(tokens[3]);
        Rarities rarity = static_cast<Rarities>(stoi(tokens[4]));
        bool isFavorite = tokens[5] == "1";
        item = new Item(name, price, durability, rarity);
        if (isFavorite) item->toggleFavorite();
    } else if (type == "HealthPotion") {
        if (tokens.size() != 7) return nullptr;
        string name = tokens[1];
        float price = stof(tokens[2]);
        int durability = stoi(tokens[3]);
        Rarities rarity = static_cast<Rarities>(stoi(tokens[4]));
        bool isFavorite = tokens[5] == "1";
        float healAmount = stof(tokens[6]);
        item = new HealthPotion();
        item->setPrice(price);
        item->setDurability(durability);
        if (isFavorite) item->toggleFavorite();
    } else if (type == "Weapon") {
        if (tokens.size() != 8) return nullptr;
        string name = tokens[1];
        float price = stof(tokens[2]);
        int durability = stoi(tokens[3]);
        Rarities rarity = static_cast<Rarities>(stoi(tokens[4]));
        bool isFavorite = tokens[5] == "1";
        float damage = stof(tokens[6]);
        int strengthBonus = stoi(tokens[7]);
        item = new Weapon(name, price, durability, rarity, damage, strengthBonus);
        if (isFavorite) item->toggleFavorite();
    } else if (type == "Armor") {
        if (tokens.size() != 8) return nullptr;
        string name = tokens[1];
        float price = stof(tokens[2]);
        int durability = stoi(tokens[3]);
        Rarities rarity = static_cast<Rarities>(stoi(tokens[4]));
        bool isFavorite = tokens[5] == "1";
        float defense = stof(tokens[6]);
        int agilityBonus = stoi(tokens[7]);
        item = new Armor(name, price, durability, rarity, defense, agilityBonus);
        if (isFavorite) item->toggleFavorite();
    }

    return item;
}

// Klasa dla przeciwników
class Enemy {
public:
    string name;
    float HP;
    float damage;
    int experience;
    int x, y;

    Enemy(string n, float hp, float dmg, int level, int xPos, int yPos)
        : name(n), HP(hp), damage(dmg), experience(20 + level * 5), x(xPos), y(yPos) {}

    void attack(float& playerHP) const {
        float actualDamage = max(1.0f, damage - (rand() % 5));
        playerHP -= actualDamage;
        cout << "\033[31m" << name << " attacks for " << actualDamage << " damage! Player HP: " << playerHP << "\n\033[37m";
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
    Player(string n, bool addInitialItems = true) : name(n), HP(100.0f), maxHP(100.0f), gold(100.0f), level(1), experience(0), nextLevelExp(100) {
        stats["strength"] = 10;
        stats["agility"] = 10;
        stats["intelligence"] = 10;

        if (addInitialItems) {
            inventory.push_back(new Weapon("Basic Sword", 50.0f, 100, common, 20.0f, 2));
            inventory.push_back(new Armor("Basic Armor", 60.0f, 100, common, 5.0f, 2));
            inventory.push_back(new HealthPotion());
            stats["strength"] += weapon.getStrengthBonus();
            stats["agility"] += armor.getAgilityBonus();
        }
    }

    ~Player() {
        for (auto item : inventory) delete item;
    }

    void heal(float amount) {
        HP = min(maxHP, HP + amount);
        cout << "\033[32mHealed for " << amount << " HP. Current HP: " << HP << "/" << maxHP << "\n\033[37m";
    }

    void showStats() const {
        cout << "\033[36m";
        cout << "Player: " << name << " | HP: " << HP << "/" << maxHP << " | Gold: " << gold << "\n";
        cout << "Level: " << level << " | Exp: " << experience << "/" << nextLevelExp << "\n";
        cout << "Weapon: ";
        weapon.display();
        cout << "\nArmor: ";
        armor.display();
        cout << "\n";
        cout << "Strength: " << stats.at("strength") << " | Agility: " << stats.at("agility") << " | Intelligence: " << stats.at("intelligence") << "\n";
        cout << "\033[37m";
    }

    void gainExperience(int exp) {
        experience += exp;
        cout << "\033[32mGained " << exp << " experience points!\n\033[37m";
        if (experience >= nextLevelExp) levelUp();
    }

    void levelUp() {
        level++;
        experience -= nextLevelExp;
        nextLevelExp = static_cast<int>(nextLevelExp * 1.5);
        maxHP += 20;
        HP = maxHP;
        stats["strength"] += 2;
        stats["agility"] += 2;
        stats["intelligence"] += 2;
        cout << "\033[32mLevel Up! You are now level " << level << "!\n\033[37m";
    }

    bool attack(Enemy& enemy) {
        float damage = weapon.getDamage() + stats.at("strength");
        cout << "\033[31mYou attack " << enemy.name << " for " << damage << " damage!\n\033[37m";
        enemy.HP -= damage;

        if (enemy.HP <= 0) {
            cout << "\033[31mEnemy " << enemy.name << " has fallen!\n";
            cout << R"(
            (x_x)
             /|\
             / \
)";
            cout << "\033[37m";
            gainExperience(enemy.experience);
            return true;
        } else {
            enemy.attack(HP);
            return false;
        }
    }

    void lootChest(int level) {
        float lootGold = static_cast<float>(rand() % 100 + 1);
        gold += lootGold;
        int expGained = 15 + level * 5;
        gainExperience(expGained);

        Item* newItem = nullptr;
        int itemType = rand() % 10;
        switch (itemType) {
        case 0: newItem = new HealthPotion(); break;
        case 1: newItem = new Weapon("Iron Sword", 100.0f, 120, uncommon, 25.0f, 4); break;
        case 2: newItem = new Armor("Leather Armor", 120.0f, 120, uncommon, 8.0f, 4); break;
        case 3: newItem = new Weapon("Magic Staff", 200.0f, 150, magic, 30.0f, 6); break;
        case 4: newItem = new Armor("Chainmail", 220.0f, 150, magic, 12.0f, 6); break;
        case 5: newItem = new Weapon("Dragon Blade", 350.0f, 200, rare, 40.0f, 8); break;
        case 6: newItem = new Armor("Plate Armor", 370.0f, 200, rare, 18.0f, 8); break;
        case 7: newItem = new Weapon("Godslayer", 500.0f, 300, legendary, 50.0f, 10); break;
        case 8: newItem = new Armor("Mythril Armor", 520.0f, 300, legendary, 25.0f, 10); break;
        case 9: newItem = new Item("Junk", 5.0f, 1, common); break;
        }

        if (newItem && inventory.size() < 16) {
            inventory.push_back(newItem);
            cout << "\033[32mFound " << lootGold << " gold, an item, and " << expGained << " EXP!\n\033[37m";
        } else {
            delete newItem;
            cout << "\033[33mInventory full! Item discarded.\n\033[37m";
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    void showInventoryGridWithCursor(int cursorRow, int cursorCol, bool isMoving = false, int selectedIndex = -1) const {
        cout << "\033[36m";
        cout << "=== INVENTORY (4x4) ===\n";
        cout << "Gold: " << gold << "\n\n";
        cout << "    1   2   3   4  \n";
        cout << "  +---+---+---+---+\n";

        for (int row = 0; row < 4; ++row) {
            cout << row + 1 << " |";
            for (int col = 0; col < 4; ++col) {
                int index = row * 4 + col;
                if (row == cursorRow && col == cursorCol) {
                    cout << (index < inventory.size() && inventory[index]->getIsFavorite() ? "\033[33m{" : "\033[31m[");
                } else {
                    cout << " ";
                }
                if (index < inventory.size()) {
                    switch (inventory[index]->getRarity()) {
                    case common: cout << "\033[90m"; break;
                    case uncommon: cout << "\033[32m"; break;
                    case magic: cout << "\033[34m"; break;
                    case rare: cout << "\033[35m"; break;
                    case legendary: cout << "\033[33m"; break;
                    }
                    cout << (inventory[index]->getIsFavorite() ? '*' : inventory[index]->getName()[0]);
                    cout << "\033[37m";
                } else {
                    cout << " ";
                }
                if (row == cursorRow && col == cursorCol) {
                    cout << (index < inventory.size() && inventory[index]->getIsFavorite() ? "\033[33m}" : "\033[31m]");
                } else {
                    cout << " ";
                }
                cout << "\033[37m|";
            }
            cout << "\n  +---+---+---+---+\n";
        }
        cout << "\033[37m";
        if (isMoving && selectedIndex >= 0 && selectedIndex < inventory.size()) {
            cout << "\033[36mMoving " << inventory[selectedIndex]->getName() << ". Press [M] to place or [X] to cancel.\n";
        }
        cout << "\n[I] Inspect  [E] Equip  [U] Use  [Q] Drop  [C] Sort  [M] Move  [X] Exit\n";
    }

    void manageInventory() {
        if (inventory.empty()) {
            cout << "\033[33mInventory is empty!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        bool isMoving = false;
        int selectedIndex = -1;
        char input;

        while (true) {
            cout << "\033[2J\033[1;1H";
            showInventoryGridWithCursor(cursorRow, cursorCol, isMoving, selectedIndex);
            input = tolower(getch());

            if (input == 'w' && cursorRow > 0) cursorRow--;
            if (input == 's' && cursorRow < 3) cursorRow++;
            if (input == 'a' && cursorCol > 0) cursorCol--;
            if (input == 'd' && cursorCol < 3) cursorCol++;

            if (input == 'i') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    cout << "\033[2J\033[1;1H\033[36m=== ITEM DETAILS ===\n";
                    inventory[index]->display();
                    cout << "\033[37m\n[F] Toggle Favorite  [X] Back\n";
                    char choice = tolower(getch());
                    if (choice == 'f') {
                        inventory[index]->toggleFavorite();
                        cout << "\033[32m" << inventory[index]->getName() << " is now "
                             << (inventory[index]->getIsFavorite() ? "marked as favorite!" : "unmarked as favorite!") << "\n";
                        cout << "\033[37mPress any key to continue...";
                        getch();
                    }
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else if (input == 'e') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    Item* item = inventory[index];
                    if (Weapon* weaponPtr = dynamic_cast<Weapon*>(item)) {
                        stats["strength"] -= weapon.getStrengthBonus();
                        weapon = *weaponPtr;
                        stats["strength"] += weapon.getStrengthBonus();
                        cout << "\033[32mWeapon equipped! Strength updated to " << stats["strength"] << "!\n";
                    } else if (Armor* armorPtr = dynamic_cast<Armor*>(item)) {
                        stats["agility"] -= armor.getAgilityBonus();
                        armor = *armorPtr;
                        stats["agility"] += armor.getAgilityBonus();
                        cout << "\033[32mArmor equipped! Agility updated to " << stats["agility"] << "!\n";
                    } else {
                        cout << "\033[33mThis item cannot be equipped!\n";
                    }
                    cout << "\033[37mPress any key to continue...";
                    getch();
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else if (input == 'u') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    inventory[index]->use(*this);
                    if (inventory[index]->getDurability() <= 0) {
                        delete inventory[index];
                        inventory.erase(inventory.begin() + index);
                    }
                    cout << "\033[37mPress any key to continue...";
                    getch();
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else if (input == 'q') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    if (inventory[index]->getIsFavorite()) {
                        cout << "\033[33mCannot drop favorite items!\n\033[37m";
                    } else {
                        cout << "\033[33mDropped " << inventory[index]->getName() << "!\n";
                        delete inventory[index];
                        inventory.erase(inventory.begin() + index);
                    }
                    cout << "\033[37mPress any key to continue...";
                    getch();
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else if (input == 'c') {
                sort(inventory.begin(), inventory.end(), [](const Item* a, const Item* b) {
                    return a->getRarity() > b->getRarity();
                });
                cout << "\033[32mInventory sorted by rarity!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            } else if (input == 'm') {
                int index = cursorRow * 4 + cursorCol;
                if (!isMoving && index < inventory.size()) {
                    selectedIndex = index;
                    isMoving = true;
                    cout << "\033[32mSelected " << inventory[selectedIndex]->getName() << " to move.\n";
                    this_thread::sleep_for(chrono::seconds(1));
                } else if (isMoving) {
                    int targetIndex = cursorRow * 4 + cursorCol;
                    if (targetIndex >= 0 && targetIndex < 16) {
                        if (targetIndex < inventory.size()) {
                            swap(inventory[selectedIndex], inventory[targetIndex]);
                            cout << "\033[32mSwapped " << inventory[targetIndex]->getName() << " with "
                                 << inventory[selectedIndex]->getName() << "!\n";
                        } else {
                            inventory.push_back(inventory[selectedIndex]);
                            inventory.erase(inventory.begin() + selectedIndex);
                            cout << "\033[32mMoved " << inventory.back()->getName() << " to new slot!\n";
                        }
                        isMoving = false;
                        cout << "\033[37mPress any key to continue...";
                        getch();
                    } else {
                        cout << "\033[33mInvalid target slot!\n\033[37m";
                        this_thread::sleep_for(chrono::seconds(1));
                    }
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            } else if (input == 'x') {
                if (isMoving) {
                    isMoving = false;
                    cout << "\033[33mMove canceled.\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                } else {
                    break;
                }
            }
        }
    }

    void addItemToInventory(Item* item) {
        if (inventory.size() < 16) {
            inventory.push_back(item);
            cout << "\033[32mItem added to inventory!\n\033[37m";
        } else {
            delete item;
            cout << "\033[33mInventory full!\n\033[37m";
        }
    }

    void spendGold(float amount) { gold -= amount; }

    float getHP() const { return HP; }
    float getGold() const { return gold; }
    int getLevel() const { return level; }
    string getName() const { return name; }

    void saveToFile() const {
        fs::create_directory("saves");
        string filename = "saves/player_" + name + ".txt";
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "\033[31mFailed to save game!\n\033[37m";
            return;
        }

        file << "Stats\n";
        file << "name:" << name << "\n";
        file << "HP:" << HP << "\n";
        file << "maxHP:" << maxHP << "\n";
        file << "gold:" << gold << "\n";
        file << "level:" << level << "\n";
        file << "experience:" << experience << "\n";
        file << "nextLevelExp:" << nextLevelExp << "\n";
        file << "strength:" << stats.at("strength") << "\n";
        file << "agility:" << stats.at("agility") << "\n";
        file << "intelligence:" << stats.at("intelligence") << "\n";

        file << "Weapon\n";
        file << weapon.serialize() << "\n";

        file << "Armor\n";
        file << armor.serialize() << "\n";

        file << "Inventory\n";
        for (const auto& item : inventory) {
            file << item->serialize() << "\n";
        }
        file << "End\n";

        file.close();
        cout << "\033[32mGame saved successfully!\n\033[37m";
    }

    static Player* loadFromFile(const string& playerName) {
        string filename = "saves/player_" + playerName + ".txt";
        ifstream file(filename);
        if (!file.is_open()) {
            return nullptr;
        }

        Player* player = new Player(playerName, false);
        string line;
        string section;

        for (auto item : player->inventory) delete item;
        player->inventory.clear();
        player->stats["strength"] -= player->weapon.getStrengthBonus();
        player->stats["agility"] -= player->armor.getAgilityBonus();
        player->weapon = Weapon();
        player->armor = Armor();

        while (getline(file, line)) {
            if (line == "Stats") {
                section = "Stats";
                continue;
            } else if (line == "Weapon") {
                section = "Weapon";
                continue;
            } else if (line == "Armor") {
                section = "Armor";
                continue;
            } else if (line == "Inventory") {
                section = "Inventory";
                continue;
            } else if (line == "End") {
                break;
            }

            if (section == "Stats") {
                size_t pos = line.find(':');
                if (pos == string::npos) continue;
                string key = line.substr(0, pos);
                string value = line.substr(pos + 1);
                if (key == "HP") player->HP = stof(value);
                else if (key == "maxHP") player->maxHP = stof(value);
                else if (key == "gold") player->gold = stof(value);
                else if (key == "level") player->level = stoi(value);
                else if (key == "experience") player->experience = stoi(value);
                else if (key == "nextLevelExp") player->nextLevelExp = stoi(value);
                else if (key == "strength") player->stats["strength"] = stoi(value);
                else if (key == "agility") player->stats["agility"] = stoi(value);
                else if (key == "intelligence") player->stats["intelligence"] = stoi(value);
            } else if (section == "Weapon") {
                Item* item = Item::deserialize(line);
                if (Weapon* weaponPtr = dynamic_cast<Weapon*>(item)) {
                    player->weapon = *weaponPtr;
                    player->stats["strength"] += player->weapon.getStrengthBonus();
                }
                delete item;
            } else if (section == "Armor") {
                Item* item = Item::deserialize(line);
                if (Armor* armorPtr = dynamic_cast<Armor*>(item)) {
                    player->armor = *armorPtr;
                    player->stats["agility"] += player->armor.getAgilityBonus();
                }
                delete item;
            } else if (section == "Inventory") {
                Item* item = Item::deserialize(line);
                if (item) {
                    player->inventory.push_back(item);
                }
            }
        }

        file.close();
        return player;
    }
};

// Implementacja HealthPotion::use po definicji klasy Player
void HealthPotion::use(Player& player) {
    player.heal(healAmount);
    durability--;
}

// Klasa dla NPC
class NPC {
private:
    string name;
    vector<Item*> inventory;
    int level;

public:
    NPC(string n, int lvl) : name(n), level(lvl) {
        generateInventory();
    }

    ~NPC() {
        for (auto item : inventory) delete item;
    }

    void generateInventory() {
        for (int i = 0; i < 5; ++i) {
            int itemType = rand() % 10;
            Item* newItem = nullptr;
            switch (itemType) {
            case 0: newItem = new HealthPotion(); break;
            case 1: newItem = new Weapon("Iron Sword", 100.0f, 120, uncommon, 25.0f, 4); break;
            case 2: newItem = new Armor("Leather Armor", 120.0f, 120, uncommon, 8.0f, 4); break;
            case 3: newItem = new Weapon("Magic Staff", 200.0f, 150, magic, 30.0f, 6); break;
            case 4: newItem = new Armor("Chainmail", 220.0f, 150, magic, 12.0f, 6); break;
            case 5: newItem = new Weapon("Dragon Blade", 350.0f, 200, rare, 40.0f, 8); break;
            case 6: newItem = new Armor("Plate Armor", 370.0f, 200, rare, 18.0f, 8); break;
            case 7: newItem = new Weapon("Godslayer", 500.0f, 300, legendary, 50.0f, 10); break;
            case 8: newItem = new Armor("Mythril Armor", 520.0f, 300, legendary, 25.0f, 10); break;
            case 9: newItem = new Item("Junk", 5.0f, 1, common); break;
            }
            if (newItem) inventory.push_back(newItem);
        }
    }

    void trade(Player& player) {
        while (true) {
            cout << "\033[2J\033[1;1H\033[36m";
            cout << "=== Trading with " << name << " ===\n";
            cout << "Your gold: " << player.getGold() << "\n\n";
            cout << "Merchant's Inventory:\n";
            for (size_t i = 0; i < inventory.size(); ++i) {
                cout << "[" << (i + 1) << "] ";
                inventory[i]->display();
                cout << "\n";
            }
            cout << "\n[B] Buy  [X] Exit\n";
            char choice = tolower(getch());

            if (choice == 'b') {
                cout << "\033[2J\033[1;1H\033[36mEnter item number to buy (1-" << inventory.size() << "): \033[37m";
                int itemIndex;
                cin >> itemIndex;
                if (itemIndex >= 1 && itemIndex <= static_cast<int>(inventory.size())) {
                    Item* item = inventory[itemIndex - 1];
                    if (player.getGold() >= item->getPrice()) {
                        player.spendGold(item->getPrice());
                        player.addItemToInventory(item); // Moved after gold check and deduction
                        inventory.erase(inventory.begin() + (itemIndex - 1));
                        cout << "\033[32mPurchased " << item->getName() << "!\n\033[37m";
                    } else {
                        cout << "\033[33mNot enough gold!\n\033[37m";
                    }
                } else {
                    cout << "\033[33mInvalid item number!\n\033[37m";
                }
                cout << "Press any key to continue...";
                cin.ignore();
                getch();
            } else if (choice == 'x') {
                break;
            }
        }
    }
};

// Klasa mapy gry
class Map {
private:
    vector<vector<char>> grid;
    int size;
    int playerX, playerY;
    vector<Enemy> enemies;
    vector<pair<int, int>> chests;
    vector<pair<int, int>> npcs;
    vector<pair<int, int>> obstacles;
    Weather weather;
    string location;

public:
    Map(int level) : size(10 + level * 2), weather(sunny), location("Map Level " + to_string(level)) {
        grid.resize(size, vector<char>(size, '.'));
        playerX = size / 2;
        playerY = size / 2;
        grid[playerY][playerX] = 'P';

        generateObstacles(level);
        generateChests(level);
        generateEnemies(level);
        generateNPCs(level);
    }

    void generateObstacles(int level) {
        obstacles.clear();
        int numObstacles = size + level * 2;
        for (int i = 0; i < numObstacles; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                obstacles.push_back(std::make_pair(x, y));
                grid[y][x] = '#';
            }
        }
    }

    void generateChests(int level) {
        chests.clear();
        int numChests = (level * 5 + 5) / 2;
        for (int i = 0; i < numChests; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                chests.push_back(std::make_pair(x, y));
                grid[y][x] = 'C';
            }
        }
    }

    void generateEnemies(int level) {
        enemies.clear();
        int numEnemies = (level * 5 + 5) / 2;
        for (int i = 0; i < numEnemies; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                enemies.emplace_back("Goblin", 30.0f + level * 5, 5.0f + level, level, x, y);
                grid[y][x] = 'E';
            }
        }
    }

    void generateNPCs(int level) {
        npcs.clear();
        int numNPCs = 2 + level / 2;
        for (int i = 0; i < numNPCs; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                npcs.push_back(std::make_pair(x, y));
                grid[y][x] = 'N';
            }
        }
    }

    void display() const {
        cout << "\033[36m";
        cout << "Location: " << location << "\n";
        cout << "Weather: ";
        switch (weather) {
        case sunny: cout << "Sunny"; break;
        case rainy: cout << "Rainy"; break;
        case cloudy: cout << "Cloudy"; break;
        default: cout << "Unknown"; break;
        }
        cout << "\n\033[37m";

        vector<vector<char>> displayGrid = grid;
        for (const auto& chest : chests) displayGrid[chest.second][chest.first] = 'C';
        for (const auto& enemy : enemies) displayGrid[enemy.y][enemy.x] = 'E';
        for (const auto& npc : npcs) displayGrid[npc.second][npc.first] = 'N';
        for (const auto& obstacle : obstacles) displayGrid[obstacle.second][obstacle.first] = '#';
        displayGrid[playerY][playerX] = 'P';

        for (auto& row : displayGrid) {
            for (char tile : row) {
                if (tile == 'P') cout << "\033[36m";
                else if (tile == 'C') cout << "\033[33m";
                else if (tile == 'E') cout << "\033[31m";
                else if (tile == 'N') cout << "\033[32m";
                else if (tile == '#') cout << "\033[90m";
                else cout << "\033[37m";
                cout << tile << " ";
            }
            cout << endl;
        }
        cout << "\033[37m";
    }

    void movePlayer(char direction) {
        int newX = playerX, newY = playerY;
        switch (tolower(direction)) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
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

    Enemy* getNearbyEnemy() {
        for (auto& enemy : enemies) {
            if (abs(enemy.x - playerX) <= 1 && abs(enemy.y - playerY) <= 1) {
                return &enemy;
            }
        }
        return nullptr;
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

    bool removeEnemy(int x, int y) {
        for (auto it = enemies.begin(); it != enemies.end(); ++it) {
            if (it->x == x && it->y == y) {
                enemies.erase(it);
                grid[y][x] = '.';
                return true;
            }
        }
        return false;
    }

    void changeWeather() {
        weather = static_cast<Weather>(rand() % 3);
    }
};

// Funkcja wyświetlająca ekran powitalny
void displayWelcomeScreen(vector<string>& playerNames, string& selectedPlayer) {
    cout << "\033[2J\033[1;1H\033[36m";
    cout << R"(
        +------------------------------------+
        |                                    |
        |         WELCOME TO THE GAME        |
        |                                    |
        +------------------------------------+
    )" << "\n\033[37m";

    cout << "\033[33mSelect a player or create a new one:\n\033[37m";
    cout << string(40, '-') << "\n";

    if (playerNames.empty()) {
        cout << "\033[31mNo saved players found.\n\033[37m";
    } else {
        for (size_t i = 0; i < playerNames.size(); ++i) {
            cout << "[" << (i + 1) << "] " << playerNames[i] << "\n";
        }
    }

    cout << string(40, '-') << "\n";
    cout << "[N] Create new player\n";
    cout << "Enter choice: ";

    char choice = getch();
    cout << "\n";

    if (choice >= '1' && choice <= '0' + playerNames.size()) {
        selectedPlayer = playerNames[choice - '1'];
    } else if (choice == 'n' || choice == 'N') {
        cout << "\033[36mEnter new player name: \033[37m";
        getline(cin, selectedPlayer);
        while (selectedPlayer.empty() || find(playerNames.begin(), playerNames.end(), selectedPlayer) != playerNames.end()) {
            cout << "\033[31mName is empty or already exists! Try again: \033[37m";
            getline(cin, selectedPlayer);
        }
    } else {
        cout << "\033[31mInvalid choice! Creating new player...\n\033[37m";
        cout << "\033[36mEnter new player name: \033[37m";
        getline(cin, selectedPlayer);
        while (selectedPlayer.empty() || find(playerNames.begin(), playerNames.end(), selectedPlayer) != playerNames.end()) {
            cout << "\033[31mName is empty or already exists! Try again: \033[37m";
            getline(cin, selectedPlayer);
        }
    }
}

// Funkcja pobierająca listę zapisanych graczy
vector<string> getSavedPlayers() {
    vector<string> playerNames;
    fs::create_directory("saves");
    for (const auto& entry : fs::directory_iterator("saves")) {
        string filename = entry.path().filename().string();
        if (filename.rfind("player_", 0) == 0 && filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
            string playerName = filename.substr(7, filename.size() - 11);
            playerNames.push_back(playerName);
        }
    }
    sort(playerNames.begin(), playerNames.end());
    return playerNames;
}

// Główna funkcja gry
int main() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif

    srand(time(0));

    vector<string> playerNames = getSavedPlayers();
    string selectedPlayer;
    displayWelcomeScreen(playerNames, selectedPlayer);

    Player* player = Player::loadFromFile(selectedPlayer);
    if (!player) {
        player = new Player(selectedPlayer);
        cout << "\033[32mNew player created: " << selectedPlayer << "\n\033[37m";
        this_thread::sleep_for(chrono::seconds(1));
    } else {
        cout << "\033[32mLoaded player: " << selectedPlayer << "\n\033[37m";
        this_thread::sleep_for(chrono::seconds(1));
    }

    Map gameMap(player->getLevel());
    int currentLevel = player->getLevel();
    int weatherChangeCounter = 0;
    int actionCounter = 0;
    const int AUTOSAVE_FREQUENCY = 10;

    while (player->getHP() > 0) {
        if (player->getLevel() > currentLevel) {
            gameMap = Map(player->getLevel());
            currentLevel = player->getLevel();
            cout << "\033[32mNew map generated for level " << currentLevel << "!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
        }

        cout << "\033[2J\033[1;1H";
        gameMap.display();
        player->showStats();
        cout << "Move (W/A/S/D), Fight (F), Loot (L), Trade (T), Inventory (I), Save (V), Quit (Q): ";

        char input = tolower(getch());
        int x, y;

        bool actionPerformed = false;

        if (input == 'q') {
            cout << "\033[33mQuitting game...\n\033[37m";
            player->saveToFile();
            break;
        } else if (input == 'v') {
            player->saveToFile();
            actionPerformed = true;
            this_thread::sleep_for(chrono::seconds(1));
        } else if (input == 'f') {
            Enemy* enemy = gameMap.getNearbyEnemy();
            if (enemy != nullptr) {
                bool enemyDefeated = player->attack(*enemy);
                if (enemyDefeated) {
                    gameMap.removeEnemy(enemy->x, enemy->y);
                }
                actionPerformed = true;
                this_thread::sleep_for(chrono::seconds(1));
            } else {
                cout << "\033[33mNo enemy nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 'l') {
            if (gameMap.isNearChest(x, y)) {
                player->lootChest(currentLevel);
                gameMap.removeChest(x, y);
                actionPerformed = true;
            } else {
                cout << "\033[33mNo chest nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 't') {
            if (gameMap.isNearNPC(x, y)) {
                NPC npc("Merchant", player->getLevel());
                npc.trade(*player);
                actionPerformed = true;
            } else {
                cout << "\033[33mNo NPC nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 'i') {
            player->manageInventory();
            actionPerformed = true;
        } else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            gameMap.movePlayer(input);
            actionPerformed = true;
        } else {
            cout << "\033[33mInvalid input!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
        }

        if (actionPerformed) {
            actionCounter++;
            if (actionCounter >= AUTOSAVE_FREQUENCY) {
                player->saveToFile();
                actionCounter = 0;
            }
        }

        if (++weatherChangeCounter % 10 == 0) gameMap.changeWeather();
    }

    if (player->getHP() <= 0) {
        cout << "\033[2J\033[1;1H\033[31m";
        cout << "Game Over! Your hero has fallen.\n";
        cout << "\033[37m";
        player->saveToFile();
    }

    delete player;
    return 0;
}
