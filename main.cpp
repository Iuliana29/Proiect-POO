#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

class Street {
    vector<int> segments;
    int level;
public:
    Street(int lvl = 1) : level(max(1, min(3, lvl))) {}
    void addSegment(const int& seg) { segments.push_back(seg); }
    int getLength() const { return static_cast<int>(segments.size()); }
    int getLevel() const { return level; }
    string getRoadType() const {
        switch (level) {
        case 1: return "Two lane road";
        case 2: return "Four lane road";
        case 3: return "Six lane road";
        default: return "Unknown";
        }
    }
    int upgradeCost() const {
        if (level == 1) return 50;
        if (level == 2) return 100;
        return -1;
    }
    bool upgradeRoad(int& money) {
        int cost = upgradeCost();
        if (cost == -1 || money < cost) return false;
        money -= cost;
        level = min(3, level + 1);
        return true;
    }
    friend ostream& operator<<(ostream& os, const Street& s) {
        os << "Street(segments=" << s.segments.size()
           << ", type=" << s.getRoadType() << ")";
        return os;
    }
};

class Building {
protected:
    string name;
    int level;
    int maxLevel;
public:
    Building(const string& n = "Building", int lvl = 1, int maxL = 3)
        : name(n), level(max(1, min(maxL, lvl))), maxLevel(maxL) {}
    const string& getName() const { return name; }
    int getLevel() const { return level; }
    int getMaxLevel() const { return maxLevel; }
    friend ostream& operator<<(ostream& os, const Building& b) {
        os << "Building(name=" << b.name << ", level=" << b.level << ")";
        return os;
    }
};

// --- ResidentialBuilding cu resurse ---
class ResidentialBuilding : public Building {
    int capacity;
    double utilitiesUsage;
    map<string,int> resourcesNeeded;
    int moneyProducedPerUpgrade;
    Street* street;
public:
    ResidentialBuilding(const string& n, int cap, double util, int lvl,
                        const map<string,int>& resNeeded, int moneyGain, Street* st)
        : Building(n, lvl, 3), capacity(cap), utilitiesUsage(util),
          resourcesNeeded(resNeeded), moneyProducedPerUpgrade(moneyGain), street(st) {}

    int getCapacity() const { return capacity * level; }
    const map<string,int>& getResourcesNeeded() const { return resourcesNeeded; }
    Street* getStreet() const { return street; }

    bool upgrade(map<string,int>& cityResources, int& money) {
        if (level >= maxLevel) return false;

        // verificăm resursele
        for (auto& [resType, qty] : resourcesNeeded) {
            if (cityResources[resType] < qty) {
                cout << "Not enough " << resType << " for upgrade of " << name << "\n";
                return false;
            }
        }

        // scădem resursele
        for (auto& [resType, qty] : resourcesNeeded) {
            cityResources[resType] -= qty;
            cout << "Used " << qty << " " << resType << " for upgrade of " << name << "\n";
        }

        level++;
        money += moneyProducedPerUpgrade;
        cout << name << " upgraded to level " << level << " (+$" << moneyProducedPerUpgrade << ")\n";
        return true;
    }

    friend ostream& operator<<(ostream& os, const ResidentialBuilding& r) {
        os << "Residential(" << static_cast<const Building&>(r)
           << ", capacity=" << r.getCapacity()
           << ", street=" << (r.street ? *r.street : Street()) << ")";
        return os;
    }
};

class UtilityBuilding : public Building {
    double coverage;
    int moneyCostPerUpgrade;
    string type;
    Street* street;
public:
    UtilityBuilding(const string& n, const string& t, double cov, int lvl, int moneyCost, Street* st)
        : Building(n, lvl, 3), coverage(cov), moneyCostPerUpgrade(moneyCost), type(t), street(st) {}
    double getCoverage() const { return coverage; }
    const string& getType() const { return type; }
    Street* getStreet() const { return street; }
    bool upgrade(int& money) {
        if (level >= maxLevel || money < moneyCostPerUpgrade) return false;
        money -= moneyCostPerUpgrade;
        level++;
        return true;
    }
    friend ostream& operator<<(ostream& os, const UtilityBuilding& u) {
        os << "Utility(" << static_cast<const Building&>(u)
           << ", type=" << u.type
           << ", coverage=" << u.getCoverage()
           << ", street=" << (u.street ? *u.street : Street()) << ")";
        return os;
    }
};

class Park : public Building {
    double populationBoost;
    Street* street;
public:
    Park(const string& n, double boost, int moneyCost, Street* st)
        : Building(n), populationBoost(boost), street(st) {}
    double getBoost() const { return populationBoost; }
    friend ostream& operator<<(ostream& os, const Park& p) {
        os << "Park(" << static_cast<const Building&>(p)
           << ", boost=" << (p.getBoost() / 100.0) << "%"
           << ", street=" << (p.street ? *p.street : Street()) << ")";
        return os;
    }
};

class City {
    string name;
    int money;
    map<string,int> cityResources;
    vector<Street> streets;
    vector<ResidentialBuilding> residential;
    vector<UtilityBuilding> utilities;
    vector<Park> parks;
public:
    City(const string& n, int startingMoney = 0) : name(n), money(startingMoney) {}

    void addStreet(const Street& s) { streets.push_back(s); }
    Street* getStreet(size_t idx) { return idx < streets.size() ? &streets[idx] : nullptr; }
    void addResidential(const ResidentialBuilding& b) { residential.push_back(b); }
    void addUtility(const UtilityBuilding& b) { utilities.push_back(b); }
    void addPark(const Park& p) { parks.push_back(p); }

    void addResource(const string& type, int amount) { cityResources[type] += amount; }
    int getMoney() const { return money; }

    void upgradeAllResidential() {
        for (auto& r : residential)
            r.upgrade(cityResources, money);
    }

    double calculateTotalPopulation() const {
        int baseCap = 0;
        for (const auto& r : residential) baseCap += r.getCapacity();
        double boost = 0.0;
        for (const auto& p : parks) boost += p.getBoost();
        return baseCap * (1.0 + boost);
    }
    bool utilitiesCoverPopulation() const {
        double totalCoverage = 0.0;
        for (const auto& u : utilities) {
            totalCoverage += u.getCoverage();
        }
        double totalPopulation = calculateTotalPopulation();
        cout << "Total population: " << totalPopulation << "\n";
        cout << "Total utility coverage: " << totalCoverage << "\n";
        return totalCoverage >= totalPopulation;
    }


    friend ostream& operator<<(ostream& os, const City& c) {
        os << "City: " << c.name << " (Money=" << c.money << ")\nResources:\n";
        for (auto& [res, qty] : c.cityResources) os << "  " << res << ": " << qty << "\n";
        os << "Streets:\n";
        for (size_t i = 0; i < c.streets.size(); ++i) os << "  [" << i << "] " << c.streets[i] << "\n";
        os << "Residential:\n";
        for (size_t i = 0; i < c.residential.size(); ++i) os << "  [" << i << "] " << c.residential[i] << "\n";
        os << "Utilities:\n";
        for (size_t i = 0; i < c.utilities.size(); ++i) os << "  [" << i << "] " << c.utilities[i] << "\n";
        os << "Parks:\n";
        for (size_t i = 0; i < c.parks.size(); ++i) os << "  [" << i << "] " << c.parks[i] << "\n";
        return os;
    }
};

// --- Main cu citire de la tastatura ---
int main() {
    cout << "=== City builder ===\n";

    string cityName;
    cout << "Enter city name: ";
    getline(cin, cityName);

    int startingMoney;
    cout << "Enter starting money: ";
    cin >> startingMoney;
    City city(cityName, startingMoney);

    // --- Resurse oraș ---
    int numResources;
    cout << "Enter number of resource types: ";
    cin >> numResources;
    for (int i = 0; i < numResources; ++i) {
        string resName; int resQty;
        cout << "Resource " << i+1 << " name: "; cin >> resName;
        cout << "Quantity: "; cin >> resQty;
        city.addResource(resName, resQty);
    }

    // --- Străzi ---
    int numStreets;
    cout << "Enter number of streets: "; cin >> numStreets;
    for (int i = 0; i < numStreets; ++i) {
        int lvl; cout << "Street " << i+1 << " level (1..3): "; cin >> lvl;
        Street s(lvl);
        int segCount; cout << "Number of segments: "; cin >> segCount;
        for (int j = 0; j < segCount; ++j) s.addSegment(j);
        city.addStreet(s);
    }

    // --- Clădiri rezidențiale ---
    int numResidential;
    cout << "Enter number of residential buildings: "; cin >> numResidential;
    cin.ignore();
    for (int i = 0; i < numResidential; ++i) {
        string resName;
        cout << "Residential building " << i+1 << " name: "; getline(cin, resName);
        int cap; cout << "Capacity: "; cin >> cap;
        int moneyGain; cout << "Money gain per upgrade: "; cin >> moneyGain;
        int numResNeeded; cout << "Number of resource types needed for upgrade: "; cin >> numResNeeded;
        map<string,int> resNeeded;
        for (int r = 0; r < numResNeeded; ++r) {
            string rName; int rQty;
            cout << "  Resource " << r+1 << " name: "; cin >> rName;
            cout << "  Quantity: "; cin >> rQty;
            resNeeded[rName] = rQty;
        }
        Street* stPtr = city.getStreet(1);
        city.addResidential(ResidentialBuilding(resName, cap, 0.5, 1, resNeeded, moneyGain, stPtr));
        cin.ignore();
    }
    int numUtil;
    cout<<"Enter amount of utility buildings: ";
    cin >> numUtil;
    for (int i = 0; i < numUtil; ++i) {
        string utilName, utilType;
        cout << "\nUtility building name: ";
        cin >> utilName;
        cout << "Type (e.g. Water, Power): ";
        cin >> utilType;
        int cov;
        cout << "Population coverage: ";
        cin >> cov;
        Street* stPtr = city.getStreet(1);
        city.addUtility(UtilityBuilding(utilName, utilType, cov, 1, 100, stPtr));
    }

    cout<<"Would you like to add a park? Y/N";
    char ans;
    cin>>ans;
    if (ans=='Y') {
        // Adăugare parc
        cout<<"Enter population boost: %";
        float boost;
        cin >> boost;
        cout << "\nAdding a small park \n";
        Street* stPtr = city.getStreet(1);
        city.addPark(Park("CentralPark", boost, 30, stPtr));
    }

    cout << "\n=== INITIAL CITY STATE ===\n";
    cout << city << "\n";

    cout << "\n--- Upgrading all residential buildings ---\n";
    city.upgradeAllResidential();

    cout << "\n=== CITY STATE AFTER UPGRADES ===\n";
    cout << city << "\n";

    cout << "Estimated total population: " << city.calculateTotalPopulation() << "\n";

    cout << "\n--- Checking utility coverage ---\n";
    if (city.utilitiesCoverPopulation()) {
        cout << "All population is covered by utilities.\n";
    } else {
        cout << "Warning: Not all population is covered by utilities!\n";
    }
    return 0;
}
