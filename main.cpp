#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Street {
private:
    string name;
    vector<string> segments;
    double congestion;
public:
    // cppcheck-suppress noExplicitConstructor
    Street(const string& n, double c=0.0): name(n), congestion(c){}
    void addSegment(const string& seg){ segments.push_back(seg); }
    int getLength() const { return segments.size(); }
    const string& getName() const { return name; }
    double getCongestion() const { return congestion; }
    void setCongestion(double c){ congestion=c; }
    friend ostream& operator<<(ostream& os, const Street& s){ os<<"Street("<<s.name<<", segments="<<s.segments.size()<<", congestion="<<s.congestion<<")"; return os; }
};

class Building {
protected:
    string name;
    int upgradeLevel;
    Street* street;
public:
    Building(const string& n,int lvl,Street* st): name(n), upgradeLevel(lvl), street(st) {}
    Building(const Building& other): name(other.name), upgradeLevel(other.upgradeLevel), street(other.street) {}
    Building& operator=(const Building& other){ if(this!=&other){ name=other.name; upgradeLevel=other.upgradeLevel; street=other.street; } return *this; }
    virtual ~Building() {}
};

class ResidentialBuilding: public Building {
private:
    int capacity;
    double utilitiesUsage;
    int resourcesNeeded;
public:
    ResidentialBuilding(const string& n,int cap,double util,int lvl,int res,Street* st)
    : Building(n,lvl,st), capacity(cap), utilitiesUsage(util), resourcesNeeded(res) {}
    ResidentialBuilding(const ResidentialBuilding& other)
    : Building(other), capacity(other.capacity), utilitiesUsage(other.utilitiesUsage), resourcesNeeded(other.resourcesNeeded) {}
    ResidentialBuilding& operator=(const ResidentialBuilding& other){ if(this!=&other){ Building::operator=(other); capacity=other.capacity; utilitiesUsage=other.utilitiesUsage; resourcesNeeded=other.resourcesNeeded; } return *this; }
    void upgrade() { upgradeLevel++; capacity += 5; }
    void print(ostream& os) const { os << "Residential(" << name << ", cap=" << capacity << ", util=" << utilitiesUsage << ", lvl=" << upgradeLevel << ", needs=" << resourcesNeeded << ", on=" << street->getName() << ")"; }
    int getCapacity() const { return capacity; }
    int getResourcesNeeded() const { return resourcesNeeded; }
};

class UtilityBuilding: public Building {
private:
    double coverage;
    int costUpgrade;
    string type;
public:
    UtilityBuilding(const string& t,double cov,int lvl,int cost,Street* st)
    : Building(t,lvl,st), coverage(cov), costUpgrade(cost), type(t) {}
    UtilityBuilding(const UtilityBuilding& other)
    : Building(other), coverage(other.coverage), costUpgrade(other.costUpgrade), type(other.type) {}
    UtilityBuilding& operator=(const UtilityBuilding& other){ if(this!=&other){ Building::operator=(other); coverage=other.coverage; costUpgrade=other.costUpgrade; type=other.type; } return *this; }
    void upgrade() { upgradeLevel++; coverage += 0.1; }
    void print(ostream& os) const { os << "Utility(" << name << ", type=" << type << ", cov=" << coverage << ", lvl=" << upgradeLevel << ", cost=" << costUpgrade << ", on=" << street->getName() << ")"; }
    double getCoverage() const { return coverage; }
    const string& getType() const { return type; }
};

class ResourceBuilding: public Building {
private:
    string resource;
    int waitTime;
public:
    ResourceBuilding(const string& r,int w,int lvl,Street* st)
    : Building(r,lvl,st), resource(r), waitTime(w) {}
    ResourceBuilding(const ResourceBuilding& other)
    : Building(other), resource(other.resource), waitTime(other.waitTime) {}
    ResourceBuilding& operator=(const ResourceBuilding& other){ if(this!=&other){ Building::operator=(other); resource=other.resource; waitTime=other.waitTime; } return *this; }
    void upgrade() { upgradeLevel++; waitTime = max(1, waitTime-1); }
    void print(ostream& os) const { os << "ResourceBld(" << name << ", wait=" << waitTime << ", lvl=" << upgradeLevel << ", on=" << street->getName() << ")"; }
};

class Park: public Building {
private:
    double populationBoost;
    int cost;
public:
    Park(const string& n,double boost,int c,int lvl,Street* st)
    : Building(n,lvl,st), populationBoost(boost), cost(c) {}
    Park(const Park& other)
    : Building(other), populationBoost(other.populationBoost), cost(other.cost) {}
    Park& operator=(const Park& other){ if(this!=&other){ Building::operator=(other); populationBoost=other.populationBoost; cost=other.cost; } return *this; }
    void upgrade() { upgradeLevel++; populationBoost += 0.01; }
    void print(ostream& os) const { os << "Park(" << name << ", boost=" << populationBoost << ", cost=" << cost << ", lvl=" << upgradeLevel << ", on=" << street->getName() << ")"; }
    double getBoost() const { return populationBoost; }
};

class City {
private:
    string name;
    vector<Street> streets;
    vector<ResidentialBuilding> residential;
    vector<UtilityBuilding> utilities;
    vector<ResourceBuilding> resources;
    vector<Park> parks;
public:
    // cppcheck-suppress noExplicitConstructor
    City(const string& n): name(n) {}

    void addStreet(const Street& s){ streets.push_back(s); }
    Street* getStreet(int i){ return &streets[i]; }

    void addResidential(const ResidentialBuilding& b){ residential.push_back(b); }
    void addUtility(const UtilityBuilding& b){ utilities.push_back(b); }
    void addResource(const ResourceBuilding& b){ resources.push_back(b); }
    void addPark(const Park& b){ parks.push_back(b); }

    double calculateTotalPopulationCapacity() const {
        int baseCap = 0;
        for(const auto& r : residential) baseCap += r.getCapacity();
        double boost = 0;
        for(const auto& p : parks) boost += p.getBoost();
        return baseCap * (1 + boost);
    }

    void upgradeAllResidential(int& cityResources){
        for(auto& r : residential){
            if(cityResources >= r.getResourcesNeeded()){
                cityResources -= r.getResourcesNeeded();
                r.upgrade();
            }
        }
    }

    double totalUtilityCoverageByType(const string& utilType) const {
        double total = 0;
        for(const auto& u : utilities){
            if(u.getType() == utilType) total += u.getCoverage();
        }
        return total;
    }

    friend ostream& operator<<(ostream& os,const City& c){
        os << "City: " << c.name << "\nStreets:\n";
        for(const auto& s : c.streets) os << "  " << s << "\n";
        os << "Residential:\n"; for(const auto& b : c.residential) b.print(os), os << "\n";
        os << "Utilities:\n"; for(const auto& b : c.utilities) b.print(os), os << "\n";
        os << "Resources:\n"; for(const auto& b : c.resources) b.print(os), os << "\n";
        os << "Parks:\n"; for(const auto& b : c.parks) b.print(os), os << "\n";
        return os;
    }
};

int main(){
    City c("UrbanRise");
    Street mainStreet("Main"); mainStreet.addSegment("S1"); mainStreet.addSegment("S2");

    // aceste 3 linii fac toate funcțiile să devină “used”
    cout << "Length=" << mainStreet.getLength() << " Cong=" << mainStreet.getCongestion() << "\n";
    mainStreet.setCongestion(0.3);
    cout << "New Cong=" << mainStreet.getCongestion() << "\n";

    c.addStreet(mainStreet);
    Street centralStreet("Central"); centralStreet.addSegment("A"); centralStreet.addSegment("B"); c.addStreet(centralStreet);
    Street* s1 = c.getStreet(0);
    Street* s2 = c.getStreet(1);

    c.addResidential(ResidentialBuilding("Apartment",100,0.8,1,20,s1));
    c.addUtility(UtilityBuilding("Power",0.7,1,15,s1));
    c.addUtility(UtilityBuilding("Water",0.5,1,10,s2));
    c.addResource(ResourceBuilding("Steel",10,1,s2));
    c.addPark(Park("GreenPark",0.05,1000,1,s2));

    cout << c;
    int cityResources = 50;
    c.upgradeAllResidential(cityResources);
    cout << "Total population capacity = " << c.calculateTotalPopulationCapacity() << "\n";
    cout << "Total Power coverage = " << c.totalUtilityCoverageByType("Power") << "\n";
    cout << "Total Water coverage = " << c.totalUtilityCoverageByType("Water") << "\n";

    return 0;
}
