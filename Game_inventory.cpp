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

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
using namespace std;

// Wyliczenia dla rzadkości przedmiotów i pogody
enum Rarities { common, uncommon, magic, rare, legendary };
enum Weather { sunny, rainy, cloudy };

// Obsługa wejścia w systemach Windows
#ifdef _WIN32
int getch() { return _getch(); }
int kbhit() { return _kbhit(); }
#else
// Pobieranie pojedynczego znaku bez oczekiwania na Enter
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

// Sprawdzanie, czy klawisz został naciśnięty
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

// Bazowa klasa dla przedmiotów w grze
class Item {
protected:
    string name;        // Nazwa przedmiotu
    float price;        // Cena w złocie
    int durability;     // Trwałość przedmiotu
    Rarities rarity;    // Rzadkość przedmiotu
public:
    Item() : name("Scrap"), price(0.1f), durability(100), rarity(common) {}
    Item(string name, float price, int durability, Rarities rarity)
        : name(name), price(price), durability(durability), rarity(rarity) {}

    // Wyświetlanie szczegółów przedmiotu podczas inspekcji
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
        cout << ")\n";
    }

    // Użycie przedmiotu (domyślnie niemożliwe)
    virtual void use(class Player& player) {
        cout << "\033[31m";
        cout << "This item cannot be used directly!\n";
        cout << "\033[37m";
    }

    // Gettery dla atrybutów przedmiotu
    string getName() const { return name; }
    float getPrice() const { return price; }
    int getDurability() const { return durability; }
    Rarities getRarity() const { return rarity; }

    // Settery dla modyfikacji atrybutów
    void setPrice(float newPrice) { price = newPrice; }
    void setDurability(int newDurability) { durability = newDurability; }
};

// Deklaracja klasy Player przed jej użyciem
class Player;

// Klasa dla mikstury zdrowia
class HealthPotion : public Item {
private:
    float healAmount; // Ilość HP przywracana przez miksturę
public:
    HealthPotion() : Item("Health Potion", 20.0f, 1, common), healAmount(30.0f) {}

    // Użycie mikstury (leczy gracza)
    void use(Player& player) override;

    // Wyświetlanie mikstury w sklepie
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

private:
    // Pomocnicza metoda do konwersji rzadkości na string
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
    float damage;       // Obrażenia zadawane przez broń
    int strengthBonus;  // Bonus do siły
public:
    Weapon() : Item("Basic Sword", 50.0f, 100, common), damage(20.0f), strengthBonus(2) {}
    Weapon(string name, float price, int durability, Rarities rarity, float damage, int strengthBonus)
        : Item(name, price, durability, rarity), damage(damage), strengthBonus(strengthBonus) {}

    // Wyświetlanie broni w sklepie
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

    // Gettery dla atrybutów broni
    float getDamage() const { return damage; }
    int getStrengthBonus() const { return strengthBonus; }

private:
    // Pomocnicza metoda do konwersji rzadkości na string
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
    float defense;     // Obrona zapewniana przez zbroję
    int agilityBonus;  // Bonus do zręczności
public:
    Armor() : Item("Basic Armor", 60.0f, 100, common), defense(5.0f), agilityBonus(2) {}
    Armor(string name, float price, int durability, Rarities rarity, float defense, int agilityBonus)
        : Item(name, price, durability, rarity), defense(defense), agilityBonus(agilityBonus) {}

    // Wyświetlanie zbroi w sklepie
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

    // Gettery dla atrybutów zbroi
    float getDefense() const { return defense; }
    int getAgilityBonus() const { return agilityBonus; }

private:
    // Pomocnicza metoda do konwersji rzadkości na string
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

// Klasa dla przeciwników
class Enemy {
public:
    string name;        // Nazwa przeciwnika
    float HP;           // Punkty życia
    float damage;       // Obrażenia zadawane przez przeciwnika
    int experience;     // Doświadczenie za pokonanie
    int x, y;           // Pozycja na mapie

    Enemy(string n, float hp, float dmg, int level, int xPos, int yPos)
        : name(n), HP(hp), damage(dmg), experience(20 + level * 5), x(xPos), y(yPos) {}

    // Atak przeciwnika na gracza
    void attack(float& playerHP) const {
        float actualDamage = max(1.0f, damage - (rand() % 5));
        playerHP -= actualDamage;
        cout << "\033[31m";
        cout << name << " attacks for " << actualDamage << " damage! Player HP: " << playerHP << "\n";
        cout << "\033[37m";
    }
};

// Klasa gracza
class Player {
private:
    string name;            // Imię gracza
    float HP;               // Aktualne punkty życia
    float maxHP;            // Maksymalne punkty życia
    float gold;             // Ilość złota
    int level;              // Poziom gracza
    int experience;         // Aktualne doświadczenie
    int nextLevelExp;       // Doświadczenie wymagane do następnego poziomu
    Weapon weapon;          // Aktualnie wyposażona broń
    Armor armor;            // Aktualnie wyposażona zbroja
    vector<Item*> inventory; // Ekwipunek gracza
    map<string, int> stats;  // Statystyki gracza (siła, zręczność, inteligencja)

public:
    // Konstruktor gracza
    Player(string n) : name(n), HP(100.0f), maxHP(100.0f), gold(100.0f), level(1), experience(0), nextLevelExp(100) {
        stats["strength"] = 10;
        stats["agility"] = 10;
        stats["intelligence"] = 10;

        // Dodanie początkowych przedmiotów do ekwipunku
        inventory.push_back(new Weapon("Basic Sword", 50.0f, 100, common, 20.0f, 2));
        inventory.push_back(new Armor("Basic Armor", 60.0f, 100, common, 5.0f, 2));
        inventory.push_back(new HealthPotion());
        // Aktualizacja statystyk na podstawie początkowego wyposażenia
        stats["strength"] += weapon.getStrengthBonus();
        stats["agility"] += armor.getAgilityBonus();
    }

    // Destruktor zwalniający pamięć przedmiotów
    ~Player() {
        for (auto item : inventory) delete item;
    }

    // Leczenie gracza
    void heal(float amount) {
        HP = min(maxHP, HP + amount);
        cout << "\033[32m";
        cout << "Healed for " << amount << " HP. Current HP: " << HP << "/" << maxHP << "\n";
        cout << "\033[37m";
    }

    // Wyświetlanie statystyk gracza
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

    // Dodawanie doświadczenia
    void gainExperience(int exp) {
        experience += exp;
        cout << "\033[32m";
        cout << "Gained " << exp << " experience points!\n";
        cout << "\033[37m";
        if (experience >= nextLevelExp) levelUp();
    }

    // Awans na nextLevelExp
    void levelUp() {
        level++;
        experience -= nextLevelExp;
        nextLevelExp = static_cast<int>(nextLevelExp * 1.5);
        maxHP += 20;
        HP = maxHP;
        stats["strength"] += 2;
        stats["agility"] += 2;
        stats["intelligence"] += 2;
        cout << "\033[32m";
        cout << "Level Up! You are now level " << level << "!\n";
        cout << "\033[37m";
    }

    // Atak gracza na przeciwnika
    bool attack(Enemy& enemy) {
        float damage = weapon.getDamage() + stats.at("strength");
        cout << "\033[31m";
        cout << "You attack " << enemy.name << " for " << damage << " damage!\n";
        cout << "\033[37m";
        enemy.HP -= damage;

        if (enemy.HP <= 0) {
            cout << "\033[31m";
            cout << "Enemy " << enemy.name << " has fallen!\n";
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

    // Otwieranie skrzyni i zdobywanie łupów
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
            cout << "\033[32m";
            cout << "Found " << lootGold << " gold, an item, and " << expGained << " EXP!\n";
            cout << "\033[37m";
        } else {
            delete newItem;
            cout << "\033[33m";
            cout << "Inventory full! Item discarded.\n";
            cout << "\033[37m";
        }
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Wyświetlanie ekwipunku w siatce 4x4
    void showInventoryGridWithCursor(int cursorRow, int cursorCol) const {
        cout << "\033[36m";
        cout << "=== INVENTORY (4x4) ===\n";
        cout << "Gold: " << gold << "\n\n";
        cout << "    1   2   3   4  \n";
        cout << "  +---+---+---+---+\n";

        for (int row = 0; row < 4; ++row) {
            cout << row + 1 << " |";
            for (int col = 0; col < 4; ++col) {
                int index = row * 4 + col;
                if (row == cursorRow && col == cursorCol) cout << "\033[31m[";
                else cout << " ";
                if (index < inventory.size()) {
                    switch (inventory[index]->getRarity()) {
                    case common: cout << "\033[90m"; break;
                    case uncommon: cout << "\033[32m"; break;
                    case magic: cout << "\033[34m"; break;
                    case rare: cout << "\033[35m"; break;
                    case legendary: cout << "\033[33m"; break;
                    }
                    cout << inventory[index]->getName()[0];
                    cout << "\033[37m";
                } else {
                    cout << " ";
                }
                if (row == cursorRow && col == cursorCol) cout << "\033[31m]";
                else cout << " ";
                cout << "\033[37m|";
            }
            cout << "\n  +---+---+---+---+\n";
        }
        cout << "\033[37m";
        cout << "\n[I] Inspect  [E] Equip  [U] Use  [Q] Drop  [C] Sort  [X] Exit\n";
    }

    // Inspekcja przedmiotów w ekwipunku
    void inspectItem() {
        if (inventory.empty()) {
            cout << "\033[33mNo items to inspect!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;
        do {
            cout << "\033[2J\033[1;1H";
            showInventoryGridWithCursor(cursorRow, cursorCol);
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
                    cout << "\033[37m\nPress any key to continue...";
                    getch();
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
        } while (input != 'x');
    }

    // Używanie przedmiotów z ekwipunku
    void useItemFromInventory() {
        if (inventory.empty()) {
            cout << "\033[33mNo items to use!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;
        do {
            cout << "\033[2J\033[1;1H";
            showInventoryGridWithCursor(cursorRow, cursorCol);
            input = tolower(getch());

            if (input == 'w' && cursorRow > 0) cursorRow--;
            if (input == 's' && cursorRow < 3) cursorRow++;
            if (input == 'a' && cursorCol > 0) cursorCol--;
            if (input == 'd' && cursorCol < 3) cursorCol++;

            if (input == 'u') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    cout << "\033[2J\033[1;1H";
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
            }
        } while (input != 'x');
    }

    // Usuwanie przedmiotów z ekwipunku
    void dropItemFromInventory() {
        if (inventory.empty()) {
            cout << "\033[33mNo items to drop!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;
        do {
            cout << "\033[2J\033[1;1H";
            showInventoryGridWithCursor(cursorRow, cursorCol);
            input = tolower(getch());

            if (input == 'w' && cursorRow > 0) cursorRow--;
            if (input == 's' && cursorRow < 3) cursorRow++;
            if (input == 'a' && cursorCol > 0) cursorCol--;
            if (input == 'd' && cursorCol < 3) cursorCol++;

            if (input == 'q') {
                int index = cursorRow * 4 + cursorCol;
                if (index < inventory.size()) {
                    cout << "\033[2J\033[1;1H";
                    cout << "\033[33mDropped " << inventory[index]->getName() << "!\n";
                    delete inventory[index];
                    inventory.erase(inventory.begin() + index);
                    cout << "\033[37mPress any key to continue...";
                    getch();
                } else {
                    cout << "\033[33mNo item in this slot!\n\033[37m";
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
        } while (input != 'x');
    }

    // Sortowanie ekwipunku po rzadkości (malejąco)
    void sortInventory() {
        sort(inventory.begin(), inventory.end(), [](const Item* a, const Item* b) {
            return a->getRarity() > b->getRarity();
        });
        cout << "\033[32mInventory sorted by rarity!\n\033[37m";
        this_thread::sleep_for(chrono::seconds(1));
    }

    // Wyposażanie przedmiotów z ekwipunku
    void equipFromInventory() {
        if (inventory.empty()) {
            cout << "\033[33mNo items to equip!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        int cursorRow = 0, cursorCol = 0;
        char input;
        do {
            cout << "\033[2J\033[1;1H";
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
            }
        } while (input != 'x');
    }

    // Dodawanie przedmiotu do ekwipunku
    void addItemToInventory(Item* item) {
        if (inventory.size() < 16) {
            inventory.push_back(item);
            cout << "\033[32mItem added to inventory!\n\033[37m";
        } else {
            delete item;
            cout << "\033[33mInventory full!\n\033[37m";
        }
    }

    // Wydawanie złota
    void spendGold(float amount) { gold -= amount; }

    // Gettery dla atrybutów gracza
    float getHP() const { return HP; }
    float getGold() const { return gold; }
    int getLevel() const { return level; }
};

// Definicja metody use dla HealthPotion
void HealthPotion::use(Player& player) {
    player.heal(healAmount);
    durability--;
    if (durability <= 0) {
        cout << "\033[31m";
        cout << "Potion used up!\n";
        cout << "\033[37m";
    }
}

// Klasa dla NPC (sprzedawcy)
class NPC {
public:
    string name;            // Imię sprzedawcy
    vector<Item*> shopItems; // Lista przedmiotów w sklepie

    // Konstruktor NPC, generuje asortyment zależny od poziomu gracza
    NPC(string n, int playerLevel) : name(n) {
        // Zawsze dodaj miksturę zdrowia
        shopItems.push_back(new HealthPotion());

        // Pula wszystkich dostępnych przedmiotów
        vector<Item*> itemPool = {
            new Weapon("Basic Sword", 50.0f, 100, common, 20.0f, 2),
            new Armor("Basic Armor", 60.0f, 100, common, 5.0f, 2),
            new Weapon("Iron Sword", 100.0f, 120, uncommon, 25.0f, 4),
            new Armor("Leather Armor", 120.0f, 120, uncommon, 8.0f, 4),
            new Weapon("Magic Staff", 200.0f, 150, magic, 30.0f, 6),
            new Armor("Chainmail", 220.0f, 150, magic, 12.0f, 6),
            new Weapon("Dragon Blade", 350.0f, 200, rare, 40.0f, 8),
            new Armor("Plate Armor", 370.0f, 200, rare, 18.0f, 8),
            new Weapon("Godslayer", 500.0f, 300, legendary, 50.0f, 10),
            new Armor("Mythril Armor", 520.0f, 300, legendary, 25.0f, 10)
        };

        // Filtruj przedmioty na podstawie poziomu mapy
        vector<Item*> filteredItems;
        for (auto item : itemPool) {
            if (playerLevel <= 2 && (item->getRarity() <= uncommon)) {
                filteredItems.push_back(item);
            } else if (playerLevel <= 4 && (item->getRarity() <= magic)) {
                filteredItems.push_back(item);
            } else if (playerLevel >= 5) {
                filteredItems.push_back(item);
            } else {
                delete item; // Usuń nieużywane przedmioty
            }
        }

        // Określ liczbę przedmiotów w sklepie (oprócz mikstury)
        int itemsToAdd = min(static_cast<int>(filteredItems.size()), 2 + playerLevel / 2);

        // Losowo wybierz przedmioty z filtrowanej puli
        std::shuffle(filteredItems.begin(), filteredItems.end(), std::default_random_engine(std::random_device()()));
        for (int i = 0; i < itemsToAdd && i < filteredItems.size(); ++i) {
            shopItems.push_back(filteredItems[i]);
        }

        // Zwolnij pamięć dla niewybranych przedmiotów
        for (size_t i = itemsToAdd; i < filteredItems.size(); ++i) {
            delete filteredItems[i];
        }
    }

    // Destruktor zwalniający pamięć przedmiotów w sklepie
    ~NPC() {
        for (auto item : shopItems) delete item;
    }

    // Wyświetlanie sklepu w formie tabeli
    void showShop() const {
        cout << "\033[2J\033[1;1H";
        cout << "\033[36m";
        cout << "=== Welcome to " << name << "'s Shop ===\n";
        cout << "\033[37m";
        cout << left;
        cout << setw(4) << "No" << setw(20) << "Item" << setw(12) << "Rarity" << setw(8) << "Price" << setw(20) << "Stats" << "\n";
        cout << string(64, '-') << "\n";

        for (size_t i = 0; i < shopItems.size(); ++i) {
            cout << setw(4) << (i + 1) << ".";
            shopItems[i]->display();
            cout << "\n";
        }

        cout << string(64, '-') << "\n";
        cout << "\033[37m";
    }

    // Handel z graczem
    void trade(Player& player) {
        char choice;
        do {
            showShop();
            cout << "\nYour gold: " << fixed << setprecision(1) << player.getGold() << "g\n";
            cout << "Select item [1-" << shopItems.size() << "] or [X] to exit: ";
            choice = getch();

            if (choice >= '1' && choice <= '0' + shopItems.size()) {
                int itemIndex = choice - '1';
                if (player.getGold() >= shopItems[itemIndex]->getPrice()) {
                    player.spendGold(shopItems[itemIndex]->getPrice());

                    Item* newItem = nullptr;
                    if (dynamic_cast<HealthPotion*>(shopItems[itemIndex])) {
                        newItem = new HealthPotion();
                    } else if (Weapon* weaponPtr = dynamic_cast<Weapon*>(shopItems[itemIndex])) {
                        newItem = new Weapon(*weaponPtr);
                    } else if (Armor* armorPtr = dynamic_cast<Armor*>(shopItems[itemIndex])) {
                        newItem = new Armor(*armorPtr);
                    } else {
                        newItem = new Item(*shopItems[itemIndex]);
                    }

                    player.addItemToInventory(newItem);
                    cout << "\033[32mPurchase successful!\n\033[37m";
                } else {
                    cout << "\033[33mNot enough gold!\n\033[37m";
                }
                cout << "Press any key to continue...";
                getch();
            } else if (choice != 'x' && choice != 'X') {
                cout << "\033[33mInvalid choice!\n\033[37m";
                cout << "Press any key to continue...";
                getch();
            }
        } while (choice != 'x' && choice != 'X');
    }
};

// Klasa mapy gry
class Map {
private:
    vector<vector<char>> grid; // Siatka mapy
    int size;                  // Rozmiar mapy
    int playerX, playerY;      // Pozycja gracza
    vector<Enemy> enemies;     // Lista przeciwników
    vector<pair<int, int>> chests; // Lista skrzyń
    vector<pair<int, int>> npcs;   // Lista NPC
    vector<pair<int, int>> obstacles; // Lista przeszkód
    Weather weather;           // Aktualna pogoda
    string location;           // Nazwa lokacji

public:
    // Konstruktor mapy
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

    // Generowanie przeszkód na mapie
    void generateObstacles(int level) {
        obstacles.clear();
        int numObstacles = size + level * 2;
        for (int i = 0; i < numObstacles; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                obstacles.push_back({ x, y });
                grid[y][x] = '#';
            }
        }
    }

    // Generowanie skrzyń na mapie
    void generateChests(int level) {
        chests.clear();
        int numChests = (level * 5 + 5) / 2;
        for (int i = 0; i < numChests; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                chests.push_back({ x, y });
                grid[y][x] = 'C';
            }
        }
    }

    // Generowanie przeciwników na mapie
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

    // Generowanie NPC na mapie
    void generateNPCs(int level) {
        npcs.clear();
        int numNPCs = 2 + level / 2;
        for (int i = 0; i < numNPCs; ++i) {
            int x = rand() % size;
            int y = rand() % size;
            if (grid[y][x] == '.' && (x != playerX || y != playerY)) {
                npcs.push_back({ x, y });
                grid[y][x] = 'N';
            }
        }
    }

    // Wyświetlanie mapy
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
                if (tile == 'P') cout << "\033[36m"; // Gracz: jasnoniebieski
                else if (tile == 'C') cout << "\033[33m"; // Skrzynia: złoty
                else if (tile == 'E') cout << "\033[31m"; // Wróg: czerwony
                else if (tile == 'N') cout << "\033[32m"; // NPC: zielony
                else if (tile == '#') cout << "\033[90m"; // Przeszkoda: szary
                else cout << "\033[37m"; // Pole: biały
                cout << tile << " ";
            }
            cout << endl;
        }
        cout << "\033[37m";
    }

    // Ruch gracza na mapie
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

    // Sprawdzanie, czy gracz jest blisko skrzyni
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

    // Pobieranie pobliskiego przeciwnika
    Enemy* getNearbyEnemy() {
        for (auto& enemy : enemies) {
            if (abs(enemy.x - playerX) <= 1 && abs(enemy.y - playerY) <= 1) {
                return &enemy;
            }
        }
        return nullptr;
    }

    // Sprawdzanie, czy gracz jest blisko NPC
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

    // Usuwanie skrzyni z mapy
    void removeChest(int x, int y) {
        for (auto it = chests.begin(); it != chests.end(); ++it) {
            if (it->first == x && it->second == y) {
                chests.erase(it);
                grid[y][x] = '.';
                break;
            }
        }
    }

    // Usuwanie przeciwnika z mapy
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

    // Zmiana pogody na mapie
    void changeWeather() {
        weather = static_cast<Weather>(rand() % 3);
    }
};

// Główna funkcja gry
int main() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif

    srand(time(0));
    Player player("Hero");
    Map gameMap(player.getLevel());
    int currentLevel = player.getLevel();
    int weatherChangeCounter = 0;

    // Główna pętla gry
    while (player.getHP() > 0) {
        if (player.getLevel() > currentLevel) {
            gameMap = Map(player.getLevel());
            currentLevel = player.getLevel();
            cout << "\033[32mNew map generated for level " << currentLevel << "!\n\033[37m";
            this_thread::sleep_for(chrono::seconds(1));
        }

        cout << "\033[2J\033[1;1H";
        gameMap.display();
        player.showStats();
        cout << "Move (W/A/S/D), Fight (F), Loot (L), Trade (T), Inventory (I): ";

        char input = tolower(getch());
        int x, y;

        if (input == 'f') {
            Enemy* enemy = gameMap.getNearbyEnemy();
            if (enemy != nullptr) {
                bool enemyDefeated = player.attack(*enemy);
                if (enemyDefeated) {
                    gameMap.removeEnemy(enemy->x, enemy->y);
                }
                this_thread::sleep_for(chrono::seconds(1));
            } else {
                cout << "\033[33mNo enemy nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 'l') {
            if (gameMap.isNearChest(x, y)) {
                player.lootChest(currentLevel);
                gameMap.removeChest(x, y);
            } else {
                cout << "\033[33mNo chest nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 't') {
            if (gameMap.isNearNPC(x, y)) {
                NPC npc("Merchant", player.getLevel());
                npc.trade(player);
            } else {
                cout << "\033[33mNo NPC nearby!\n\033[37m";
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else if (input == 'i') {
            char inventoryChoice;
            do {
                cout << "\033[2J\033[1;1H";
                player.showInventoryGridWithCursor(0, 0);
                inventoryChoice = tolower(getch());
                switch (inventoryChoice) {
                case 'i': player.inspectItem(); break;
                case 'e': player.equipFromInventory(); break;
                case 'u': player.useItemFromInventory(); break;
                case 'q': player.dropItemFromInventory(); break;
                case 'c': player.sortInventory(); break;
                }
            } while (inventoryChoice != 'x');
        } else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            gameMap.movePlayer(input);
        }

        if (++weatherChangeCounter % 10 == 0) gameMap.changeWeather();
    }

    cout << "\033[2J\033[1;1H\033[31m";
    cout << "Game Over! Your hero has fallen.\n";
    cout << "\033[37m";
    return 0;
}
