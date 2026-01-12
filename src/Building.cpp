#include "../include/Building.hpp"
#include "../include/Street.hpp"
#include "../include/Exceptions.hpp"
#include "../include/BuildingVisitor.hpp"
#include "../include/Factory.hpp"

void ResidentialBuilding::accept(BuildingVisitor& v) { v.visit(*this); }
void UtilityBuilding::accept(BuildingVisitor& v) { v.visit(*this); }
void Park::accept(BuildingVisitor& v) { v.visit(*this); }
void CommercialBuilding::accept(BuildingVisitor& v) { v.visit(*this); }
int Building::buildingCount_ = 0;

// constructor baza pentru cladire
Building::Building(std::string name, int lvl, int maxL) : name_(std::move(name)), level_(std::max(1, std::min(maxL, lvl))),maxLevel_(maxL) {
    ++buildingCount_;
}

// destructor – decrementeaza contorul global
Building::~Building() {
    --buildingCount_;
}

void Building::print(std::ostream& os) const {
    printImpl(os);
}

// operator << pentru afisare
std::ostream& operator<<(std::ostream& os, const Building& b) {
    b.print(os);
    return os;
}

const std::string& Building::name() const noexcept {
    return name_;
}

int Building::level() const noexcept {
    return level_;
}

int Building::buildingCount() noexcept {
    return buildingCount_;
}

BuildingCreator& BuildingCreator::instance() {
    static BuildingCreator inst;
    return inst;
}

// inregistrare tip cladire -> functie de creare
void BuildingCreator::registerCreator(const std::string& id, Creator c) {
    registry_[id] = std::move(c);
}

// creaza cladire din registru dupa id
std::shared_ptr<Building> BuildingCreator::create( const std::string& id, const std::string& name, const std::vector<std::string>& params,Street* street) const {
    auto it = registry_.find(id);
    if (it == registry_.end())
        throw CityException("Unknown building type: " + id);
    return it->second(name, params, street);
}

ResidentialBuilding::ResidentialBuilding( const std::string& n, int cap, int lvl, const std::map<std::string,int>& resNeeded, int moneyPerUpgrade, Street* st)
    : Building(n, lvl, 3), capacityBase_(cap), resourcesNeeded_(resNeeded), moneyProducedPerUpgrade_(moneyPerUpgrade), street_(st) {
    if (capacityBase_ <= 0)
        throw CityException("Residential must have positive base capacity");
}

// afisare informatii cladire rezidentiala
void ResidentialBuilding::printImpl(std::ostream& os) const {
    os << "Residential(name=" << name_ << ", level=" << level_ << ", capacity=" << capacityEffect() << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

// upgrade – consuma resurse si produce bani
void ResidentialBuilding::upgrade(ResourcePool<int>& cityResources, int& money) {
    if (level_ >= maxLevel_) return;

    for (const auto& kv : resourcesNeeded_) {
        if (cityResources.get(kv.first) < kv.second)
            throw InsufficientResourceException(kv.first);
    }
    for (const auto& kv : resourcesNeeded_) {
        cityResources.consume(kv.first, kv.second);
    }

    ++level_;
    money += moneyProducedPerUpgrade_;
}


// clona polimorfa
std::shared_ptr<Building> ResidentialBuilding::clone_shared() const {
    return std::make_shared<ResidentialBuilding>(*this);
}

// capacitatea in functie de nivel
int ResidentialBuilding::capacityEffect() const {
    return capacityBase_ * level_;
}


UtilityBuilding::UtilityBuilding(
    const std::string& n,
    std::string t,
    double cov,
    int lvl,
    int moneyCost,
    Street* st)
    : Building(n, lvl, 3),
      coverage_(cov),
      moneyCostPerUpgrade_(moneyCost),
      type_(std::move(t)),
      street_(st) {}

void UtilityBuilding::printImpl(std::ostream& os) const {
    os << "Utility(name=" << name_ << ", type=" << type_ << ", level=" << level_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void UtilityBuilding::upgrade(ResourcePool<int>&, int& money) {
    if (level_ >= maxLevel_) return;
    if (money < moneyCostPerUpgrade_)
        throw CityException("Not enough money to upgrade utility");
    money -= moneyCostPerUpgrade_;
    ++level_;
}

// clona polimorfa
std::shared_ptr<Building> UtilityBuilding::clone_shared() const {
    return std::make_shared<UtilityBuilding>(*this);
}

// efect asupra capacitatii
int UtilityBuilding::capacityEffect() const {
    return static_cast<int>(coverage_) * level_;
}

Park::Park(const std::string& n, double boost, int cost, Street* st)
    : Building(n, 1, 2),
      populationBoost_(boost),
      moneyCost_(cost),
      street_(st) {}

// afisare parc
void Park::printImpl(std::ostream& os) const {
    os << "Park(name=" << name_ << ", level=" << level_ << ", boost=" << populationBoost_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void Park::upgrade(ResourcePool<int>&, int&) {
    if (level_ < maxLevel_) ++level_;
}

// clona polimorfa
std::shared_ptr<Building> Park::clone_shared() const {
    return std::make_shared<Park>(*this);
}
int Park::capacityEffect() const {
    return static_cast<int>(populationBoost_) * level_;
}

// cost de constructie
int Park::cost() const noexcept {
    return moneyCost_;
}


CommercialBuilding::CommercialBuilding(
    const std::string& n,
    int baseCustomers,
    int lvl,
    Street* st)
    : Building(n, lvl, 4),
      customersPerLevel_(baseCustomers),
      street_(st) {

    if (baseCustomers < 0)
        throw CityException("Commercial base customers must be non-negative");
}

// afisare cladire comerciala
void CommercialBuilding::printImpl(std::ostream& os) const {
    os << "Commercial(name=" << name_ << ", level=" << level_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

// upgrade – cost fix in functie de nivel
void CommercialBuilding::upgrade(ResourcePool<int>&, int& money) {
    int cost = 20 * level_;
    if (money < cost)
        throw CityException("Not enough money to upgrade commercial building");
    money -= cost;
    ++level_;
}

// clona polimorfa
std::shared_ptr<Building> CommercialBuilding::clone_shared() const {
    return std::make_shared<CommercialBuilding>(*this);
}

// capacitate = clienti per nivel * nivel
int CommercialBuilding::capacityEffect() const {
    return customersPerLevel_ * level_;
}

namespace {

// inregistrare tip "residential"
[[maybe_unused]] const bool residential_registered = [](){
    BuildingCreator::instance().registerCreator
    ("residential",
     [](const std::string& name,
        const std::vector<std::string>& params,
        Street* st) -> std::shared_ptr<Building>
        {
            int cap = !params.empty() ? std::stoi(params[0]) : 10;
            int lvl = params.size() > 1 ? std::stoi(params[1]) : 1;
            int money = params.size() > 2 ? std::stoi(params[2]) : 20;
            std::map<std::string,int> needed{{"wood",10},{"stone",5}};
            return std::make_shared<ResidentialBuilding>(name, cap, lvl, needed, money, st);
        }
    );
    return true;
}();

// inregistrare tip "utility"
[[maybe_unused]] const bool utility_registered = [](){
    BuildingCreator::instance().registerCreator
    (
        "utility",
        [](const std::string& name,
           const std::vector<std::string>& params,
           Street* st) -> std::shared_ptr<Building>
        {
            std::string t = !params.empty()? params[0] : "Water";
            double cov = params.size() > 1 ? std::stod(params[1]) : 100.0;
            int lvl = params.size() > 2 ? std::stoi(params[2]) : 1;
            int cost = params.size() > 3 ? std::stoi(params[3]) : 50;
            return std::make_shared<UtilityBuilding>(name, t, cov, lvl, cost, st);
        }
    );
    return true;
}();

// inregistrare tip "park"
[[maybe_unused]] const bool park_registered = [](){
    BuildingCreator::instance().registerCreator
    (
        "park",
        [](const std::string& name,
           const std::vector<std::string>& params,
           Street* st) -> std::shared_ptr<Building>
        {
            double boost = !params.empty() ? std::stod(params[0]) : 10.0;
            int cost = params.size() > 1 ? std::stoi(params[1]) : 30;
            return std::make_shared<Park>(name, boost, cost, st);
        }
    );
    return true;
}();

// inregistrare tip "commercial"
[[maybe_unused]] const bool commercial_registered = [](){
    BuildingCreator::instance().registerCreator
    (
        "commercial", [](const std::string& name, const std::vector<std::string>& params, Street* st) -> std::shared_ptr<Building>
        {
            int baseC = !params.empty() ? std::stoi(params[0]) : 50;
            int lvl = params.size() > 1 ? std::stoi(params[1]) : 1;
            return std::make_shared<CommercialBuilding>(name, baseC, lvl, st);
        }
    );
    return true;
}();

}

// constructor – slot cu cladire
Slot::Slot(std::shared_ptr<Building> b) noexcept : building_(std::move(b)) {}

// seteaza cladirea din slot
void Slot::setBuilding(std::shared_ptr<Building> b) noexcept {
    building_ = std::move(b);
}

// upgrade pornind de la pointer de baza
void Slot::upgradeSlot(ResourcePool<int>& resources, int& money) const {
    if (!building_) throw CityException("No building in slot to upgrade");
    building_->upgrade(resources, money);
}

// afisare slot
void Slot::show(std::ostream& os) const {
    if (building_) {
        building_->print(os);
    } else {
        os << "Empty slot";
    }
}

// capacitatea cladirii din slot
 int Slot::capacity() const noexcept {
    if (!building_) return 0;
    return building_->capacityEffect();
}

// acces la cladire
std::shared_ptr<Building> Slot::building() const noexcept {
    return building_;
}
