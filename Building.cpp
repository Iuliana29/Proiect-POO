#include "Building.hpp"
#include "Street.hpp"
#include "Exceptions.hpp"
#include <algorithm>

int Building::buildingCount_ = 0;

Building::Building(std::string name, int lvl, int maxL)
    : name_(std::move(name)),
      level_(std::max(1, std::min(maxL, lvl))),
      maxLevel_(maxL) {
    ++buildingCount_;
}

Building::~Building() {
    --buildingCount_;
}

void Building::print(std::ostream& os) const {
    printImpl(os);
}

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

BuildingFactory& BuildingFactory::instance() {
    static BuildingFactory inst;
    return inst;
}

void BuildingFactory::registerFactory(const std::string& id, Creator c) {
    std::lock_guard<std::mutex> lock(mtx_);
    registry_[id] = std::move(c);
}

std::shared_ptr<Building> BuildingFactory::create(const std::string& id, const std::string& name, const std::vector<std::string>& params, Street* street) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = registry_.find(id);
    if (it == registry_.end())
        throw CityException("Unknown building type: " + id);
    return it->second(name, params, street);
}

ResidentialBuilding::ResidentialBuilding(const std::string& n, int cap, int lvl, const std::map<std::string,int>& resNeeded, int moneyPerUpgrade, Street* st)
    : Building(n, lvl, 3), capacityBase_(cap), resourcesNeeded_(resNeeded), moneyProducedPerUpgrade_(moneyPerUpgrade), street_(st) {
    if (capacityBase_ <= 0)
        throw CityException("Residential must have positive base capacity");
}

void ResidentialBuilding::printImpl(std::ostream& os) const {
    os << "Residential(name=" << name_ << ", level=" << level_ << ", capacity=" << capacityEffect() << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void ResidentialBuilding::upgrade(std::map<std::string,int>& cityResources, int& money) {
    if (level_ >= maxLevel_)
        return;
    for (const auto& kv : resourcesNeeded_) {
        if (cityResources[kv.first] < kv.second)
            throw InsufficientResourceException(kv.first);
    }
    for (auto& kv : resourcesNeeded_)
        cityResources[kv.first] -= kv.second;
    ++level_;
    money += moneyProducedPerUpgrade_;
}

std::shared_ptr<Building> ResidentialBuilding::clone_shared() const {
    return std::make_shared<ResidentialBuilding>(*this);
}

int ResidentialBuilding::capacityEffect() const {
    return capacityBase_ * level_;
}

UtilityBuilding::UtilityBuilding(const std::string& n, std::string t, double cov, int lvl, int moneyCost, Street* st)
    : Building(n, lvl, 3),coverage_(cov), moneyCostPerUpgrade_(moneyCost), type_(std::move(t)), street_(st) {}

void UtilityBuilding::printImpl(std::ostream& os) const {
    os << "Utility(name=" << name_ << ", type=" << type_ << ", level=" << level_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void UtilityBuilding::upgrade(std::map<std::string,int>&, int& money) {
    if (level_ >= maxLevel_) return;
    if (money < moneyCostPerUpgrade_)
        throw CityException("Not enough money to upgrade utility");
    money -= moneyCostPerUpgrade_;
    ++level_;
}

std::shared_ptr<Building> UtilityBuilding::clone_shared() const {
    return std::make_shared<UtilityBuilding>(*this);
}

int UtilityBuilding::capacityEffect() const {
    return static_cast<int>(coverage_) * level_;
}

Park::Park(const std::string& n, double boost, int cost, Street* st) : Building(n, 1, 2), populationBoost_(boost), moneyCost_(cost), street_(st) {}

void Park::printImpl(std::ostream& os) const {
    os << "Park(name=" << name_ << ", level=" << level_ << ", boost=" << populationBoost_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void Park::upgrade(std::map<std::string,int>&, int&) {
    if (level_ < maxLevel_) ++level_;
}

std::shared_ptr<Building> Park::clone_shared() const {
    return std::make_shared<Park>(*this);
}

int Park::capacityEffect() const {
    return static_cast<int>(populationBoost_) * level_;
}

int Park::cost() const noexcept {
    return moneyCost_;
}

CommercialBuilding::CommercialBuilding(const std::string& n, int baseCustomers, int lvl, Street* st)
    : Building(n, lvl, 4), customersPerLevel_(baseCustomers), street_(st) {
    if (baseCustomers < 0)
        throw CityException("Commercial base customers must be non-negative");
}

void CommercialBuilding::printImpl(std::ostream& os) const {
    os << "Commercial(name=" << name_ << ", level=" << level_ << ")";
    if (street_) {
        os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
    }
}

void CommercialBuilding::upgrade(std::map<std::string,int>&, int& money) {
    int cost = 20 * level_;
    if (money < cost)
        throw CityException("Not enough money to upgrade commercial building");
    money -= cost;
    ++level_;
}

std::shared_ptr<Building> CommercialBuilding::clone_shared() const {
    return std::make_shared<CommercialBuilding>(*this);
}

int CommercialBuilding::capacityEffect() const {
    return customersPerLevel_ * level_;
}

namespace {
[[maybe_unused]] const bool residential_registered = [](){
    BuildingFactory::instance().registerFactory
    ("residential",[](const std::string& name,const std::vector<std::string>& params,Street* st) -> std::shared_ptr<Building>
        {
            int cap   = !params.empty() ? std::stoi(params[0]) : 10;
            int lvl   = params.size() > 1 ? std::stoi(params[1]) : 1;
            int money = params.size() > 2 ? std::stoi(params[2]) : 20;
            std::map<std::string,int> needed{{"wood",10},{"stone",5}};
            return std::make_shared<ResidentialBuilding>(name, cap, lvl, needed, money, st);
        }
    );
    return true;
}();

[[maybe_unused]] const bool utility_registered = [](){
    BuildingFactory::instance().registerFactory
    (
    "utility",[](const std::string& name, const std::vector<std::string>& params, Street* st) -> std::shared_ptr<Building>
        {
            std::string t   = !params.empty() ? params[0] : "Water";
            double cov      = params.size() > 1 ? std::stod(params[1]) : 100.0;
            int lvl         = params.size() > 2 ? std::stoi(params[2]) : 1;
            int cost        = params.size() > 3 ? std::stoi(params[3]) : 50;
            return std::make_shared<UtilityBuilding>(name, t, cov, lvl, cost, st);
        }
    );
    return true;
}();

[[maybe_unused]] const bool park_registered = [](){
    BuildingFactory::instance().registerFactory
    (
        "park",[](const std::string& name,const std::vector<std::string>& params,Street* st) -> std::shared_ptr<Building>
        {
            double boost = !params.empty() ? std::stod(params[0]) : 10.0;
            int cost     = params.size() > 1 ? std::stoi(params[1]) : 30;
            return std::make_shared<Park>(name, boost, cost, st);
        }
    );
    return true;
}();

[[maybe_unused]] const bool commercial_registered = [](){
    BuildingFactory::instance().registerFactory
    (
        "commercial",[](const std::string& name,const std::vector<std::string>& params,Street* st) -> std::shared_ptr<Building>
        {
            int baseC = !params.empty() ? std::stoi(params[0]) : 50;
            int lvl   = params.size() > 1 ? std::stoi(params[1]) : 1;
            return std::make_shared<CommercialBuilding>(name, baseC, lvl, st);
        }
    );
    return true;
}();
}

Block::Block(std::shared_ptr<Building> b) noexcept : building_(std::move(b)) {}

void Block::setBuilding(std::shared_ptr<Building> b) noexcept {
    building_ = std::move(b);
}

void Block::upgradeBlock(std::map<std::string,int>& resources, int& money) const {
    if (!building_) throw CityException("No building in block to upgrade");
    building_->upgrade(resources, money);
}

void Block::show(std::ostream& os) const {
    if (building_) {
        building_->print(os);
    } else {
        os << "Empty block";
    }
}

int Block::capacity() const noexcept {
    if (!building_) return 0;
    return building_->capacityEffect();
}

std::shared_ptr<Building> Block::building() const noexcept {
    return building_;
}
