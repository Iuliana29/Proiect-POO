#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <limits> // <-- Added for robust input clearing

using namespace std;

// Forward declaration needed for the robust cin.ignore fix
// Used to clear the input buffer of any leftover characters, including the newline
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

class Street {
    vector<int> segments;
    int level;
public:
    explicit Street(int lvl = 1) : level(max(1, min(3, lvl))) {}
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
    explicit Building(string n = "Building", int lvl = 1, int maxL = 3)
        : name(std::move(n)), level(max(1, min(maxL, lvl))), maxLevel(maxL) {}
    const string& getName() const { return name; }
    int getLevel() const { return level; }
    int getMaxLevel() const { return maxLevel; }
    friend ostream& operator<<(ostream& os, const Building& b) {
        os << "Building(name=" << b.name << ", level=" << b.level << ")";
        return os;
    }
};

class ResidentialBuilding : public Building {
    int capacity;
    map<string,int> resourcesNeeded;
    int moneyProducedPerUpgrade;
    Street* street;
public:
    ResidentialBuilding(const string& n, int cap, int lvl,
                        const map<string,int>& resNeeded, int moneyGain, Street* st)
        : Building(n, lvl, 3), capacity(cap),
          resourcesNeeded(resNeeded), moneyProducedPerUpgrade(moneyGain), street(st) {}

    int getCapacity() const { return capacity * level; }
    const map<string,int>& getResourcesNeeded() const { return resourcesNeeded; }
    Street* getStreet() const { return street; }

    bool upgrade(map<string,int>& cityResources, int& money) {
        if (level >= maxLevel) return false;

        for (auto& [resType, qty] : resourcesNeeded) {
            if (cityResources[resType] < qty) {
                cout << "Not enough " << resType << " for upgrade of " << name << "\n";
                return false;
            }
        }

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
    UtilityBuilding(const string& n, string t, double cov, int lvl, int moneyCost, Street* st)
        : Building(n, lvl, 3), coverage(cov), moneyCostPerUpgrade(moneyCost), type(std::move(t)), street(st) {}
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
    int moneyCost;
    Street* street;
public:
    Park(const string& n, double boost, int mCost, Street* st)
        : Building(n), populationBoost(boost), moneyCost(mCost), street(st) {}

    double getBoost() const { return populationBoost; }
    int getCost() const { return moneyCost; }

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
    explicit City(string n, int startingMoney = 0) : name(std::move(n)), money(startingMoney) {}

    void addStreet(const Street& s) { streets.push_back(s); }
    Street* getStreet(size_t idx) { return idx < streets.size() ? &streets[idx] : nullptr; }
    void addResidential(const ResidentialBuilding& b) { residential.push_back(b); }
    void addUtility(const UtilityBuilding& b) { utilities.push_back(b); }
    void addPark(const Park& p) { parks.push_back(p); money -= p.getCost(); }

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
        return baseCap * (1.0 + (boost / 100.0)); // Adjusted boost to be a percentage
    }

    bool utilitiesCoverPopulation() const {
        double totalCoverage = 0.0;
        for (const auto& u : utilities) totalCoverage += u.getCoverage();
        double totalPopulation = calculateTotalPopulation();
        cout << "Total population: " << totalPopulation << "\n";
        cout << "Total utility coverage: " << totalCoverage << "\n";
        return totalCoverage >= totalPopulation;
    }

    // --- Functii pentru limitarea cladirilor ---
    int getMaxBuildings() const {
        int totalSegments = 0;
        // The original code used a redundant pointer vector. Fixed to iterate over streets vector directly.
        for (const auto& s : streets) totalSegments += s.getLength();
        return totalSegments * 2;
    }

    int getRemainingBuildingSlots() const {
        int totalBuildings = static_cast<int>(residential.size() + utilities.size() + parks.size());
        return getMaxBuildings() - totalBuildings;
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

int main() {
    cout << " City builder \n";

    string cityName;
    cout << "Enter city name: ";
    getline(cin, cityName);

    int startingMoney;
    cout << "Enter starting money: ";
    cin >> startingMoney;
    clearInputBuffer(); // <-- FIX: Clear buffer after reading int startingMoney
    City city(cityName, startingMoney);

    int numResources;
    cout << "Enter number of resource types: ";
    cin >> numResources;
    clearInputBuffer(); // <-- FIX: Clear buffer after reading int numResources
    for (int i = 0; i < numResources; ++i) {
        string resName; int resQty;
        cout << "Resource " << i+1 << " name: "; cin >> resName;
        cout << "Quantity: "; cin >> resQty;
        // No buffer clear needed here because we are not immediately following with getline
    }

    int numStreets;
    cout << "Enter number of streets: "; cin >> numStreets;
    clearInputBuffer(); // <-- FIX: Clear buffer after reading int numStreets
    for (int i = 0; i < numStreets; ++i) {
        int lvl; cout << "Street " << i+1 << " level (1..3): "; cin >> lvl;
        Street s(lvl);
        int segCount; cout << "Number of segments: "; cin >> segCount;
        for (int j = 0; j < segCount; ++j) s.addSegment(j);
        city.addStreet(s);
    }
    clearInputBuffer(); // <-- FIX: Clear buffer after the final street input

    int maxBuildings = city.getMaxBuildings();
    cout << "Maximum number of buildings allowed in city: " << maxBuildings << "\n";

    // Adaugare cladiri rezidentiale
    int numResidential;
    cout << "Enter number of residential buildings: "; cin >> numResidential;
    clearInputBuffer(); // <-- FIX: Clear buffer after reading int numResidential
    if (numResidential > city.getRemainingBuildingSlots()) {
        cout << "Warning: Cannot add " << numResidential
              << " buildings. Remaining slots: " << city.getRemainingBuildingSlots() << "\n";
        numResidential = city.getRemainingBuildingSlots();
    }

    for (int i = 0; i < numResidential; ++i) {
        string resName;
        cout << "Residential building " << i+1 << " name: "; getline(cin, resName); // Now reads correctly
        int cap; cout << "Capacity: "; cin >> cap;
        int moneyGain; cout << "Money gain per upgrade: "; cin >> moneyGain;
        int numResNeeded; cout << "Number of resource types needed for upgrade: "; cin >> numResNeeded;

        map<string,int> resNeeded;
        for (int r = 0; r < numResNeeded; ++r) {
            string rName; int rQty;
            cout << "  Resource " << r+1 << " name: "; cin >> rName;
            cout << "  Quantity: "; cin >> rQty;
        }

        Street* stPtr = city.getStreet(0);
        city.addResidential(ResidentialBuilding(resName, cap, 1, resNeeded, moneyGain, stPtr));
        clearInputBuffer(); // <-- FIX: Clear buffer after the final numerical input for this building
    }

    // Adaugare cladiri utilitare
    int numUtil;
    cout << "Enter number of utility buildings: "; cin >> numUtil;
    clearInputBuffer(); // <-- FIX: Clear buffer after reading int numUtil
    if (numUtil > city.getRemainingBuildingSlots()) {
        cout << "Warning: Cannot add " << numUtil
              << " buildings. Remaining slots: " << city.getRemainingBuildingSlots() << "\n";
        numUtil = city.getRemainingBuildingSlots();
    }
    for (int i = 0; i < numUtil; ++i) {
        string utilName, utilType;
        cout << "\nUtility building name: "; cin >> utilName;
        cout << "Type (e.g. Water, Power): "; cin >> utilType;
        int cov;
        cout << "Population coverage: "; cin >> cov;
        Street* stPtr = city.getStreet(0);
        city.addUtility(UtilityBuilding(utilName, utilType, cov, 1, 100, stPtr));
        clearInputBuffer(); // <-- FIX: Clear buffer after the final numerical input for this building
    }

    // --- Adaugare parc ---
    cout << "Would you like to add a park? Y/N: ";
    char ans; cin >> ans;
    // No clearInputBuffer() here, as it's followed by a float input, not a getline

    if ((ans == 'Y' || ans == 'y') && city.getRemainingBuildingSlots() > 0) {
        cout << "Enter population boost (%): ";
        float boost; cin >> boost;
        clearInputBuffer(); // <-- FIX: Clear buffer after reading float boost
        Street* stPtr = city.getStreet(0);
        Park newPark("CentralPark", boost, 30, stPtr);
        city.addPark(newPark);
        cout << "Park added. Remaining building slots: " << city.getRemainingBuildingSlots() << "\n";
    } else if (city.getRemainingBuildingSlots() <= 0) {
        cout << "Cannot add park: no remaining building slots.\n";
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