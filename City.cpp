#include "City.hpp"
#include "Exceptions.hpp"
#include <iostream>
#include <utility>

City::City(std::string n, int startingMoney) noexcept: name_(std::move(n)), money_(startingMoney) {}

City::City(const City& other): name_(other.name_),money_(other.money_),resources_(other.resources_),streets_(other.streets_) {
    buildings_.reserve(other.buildings_.size());
    for (const auto& b : other.buildings_)
        buildings_.push_back(b->clone_shared());
}

City& City::operator=(City other) noexcept {
    swap(*this, other);
    return *this;
}
// schimba doua orase intre ele
void swap(City& a, City& b) noexcept {
    using std::swap;
    swap(a.name_, b.name_);
    swap(a.money_, b.money_);
    swap(a.resources_, b.resources_);
    swap(a.streets_, b.streets_);
    swap(a.buildings_, b.buildings_);
}

void City::addStreet(const Street& s) {
    streets_.push_back(s);
}

Street* City::getStreet(std::size_t idx) {
    if (idx >= streets_.size())
        return nullptr;
    return &streets_[idx];
}

const Street* City::getStreet(std::size_t idx) const {
    if (idx >= streets_.size())
        return nullptr;
    return &streets_[idx];
}

void City::addResource(const std::string& type, int amount) {
    if (amount < 0)
        throw CityException("Cannot add negative resource");
    resources_[type] += amount;
}

void City::setMoney(int m) noexcept {
    money_ = m;
}

int City::money() const noexcept {
    return money_;
}
// creaza si adauga cladire prin creator
void City::addBuilding(const std::string& typeId, const std::string& name, const std::vector<std::string>& params, std::size_t streetIdx) {
    Street* st = getStreet(streetIdx);
    auto b = BuildingCreator::instance().create(typeId, name, params, st);
    if (auto p = std::dynamic_pointer_cast<Park>(b)) {
        if (money_ < p->cost()) throw CityException("Not enough money for park");
        money_ -= p->cost();
    }
    buildings_.push_back(std::move(b));
}
// upgrade pentru toate cladirile (polimorfism)
void City::upgradeAllBuildings() {
    for (auto& b : buildings_) {
        try {
            b->upgrade(resources_, money_);
        } catch (const InsufficientResourceException& e) {
            std::cout << "Resource error while upgrading " << b->name()<< ": " << e.what() << "\n";
        }
    }
}
// upgrade doar pentru cladiri rezidentiale (dynamic_cast)
void City::upgradeResidentialOnly() {
    for (auto& b : buildings_) {
        if (auto r = std::dynamic_pointer_cast<ResidentialBuilding>(b)) {
            try {
                r->upgrade(resources_, money_);
            } catch (const InsufficientResourceException& e) {
                std::cout << "Residential upgrade failed for " << r->name()<< ": " << e.what() << "\n";
            }
        }
    }
}

int City::maxBuildings() const noexcept {
    int totalSegments = 0;
    for (const auto& s : streets_) totalSegments += s.length();
    return totalSegments * 2;
}
// adauga cladire direct, fara creator
void City::addBuildingDirect(std::shared_ptr<Building> b) {
    if (static_cast<int>(buildings_.size()) >= maxBuildings())
        throw LimitExceededException();
    buildings_.push_back(std::move(b));
}

int City::remainingSlots() const noexcept {
    return maxBuildings() - static_cast<int>(buildings_.size());
}

void City::printSummary() const {
    std::cout << "City: " << name_ << " (Money=" << money() << ", BuildingsTotal=" << Building::buildingCount() << ", MaxBuildings=" << maxBuildings() << ", RemainingSlots=" << remainingSlots() << ", TotalCapacity=" << totalCapacity() << ")\nResources:\n";
    for (const auto& kv : resources_)
        std::cout << "  " << kv.first << ": " << kv.second << "\n";
    std::cout << "Streets:\n";
    for (std::size_t i = 0; i < streets_.size(); ++i) {
        const Street& st = streets_[i];
        std::cout << " [" << i << "] " << st << " (type=" << st.roadType()<< ", level="  << st.level() << ", length=" << st.length() << ")\n";
    }
    std::cout << "Buildings:\n";
    for (std::size_t i = 0; i < buildings_.size(); ++i)
        std::cout << " [" << i << "] " << *buildings_[i] << "\n";
}

int City::totalCapacity() const noexcept {
    int tot = 0;
    for (const auto& b : buildings_)
        tot += b->capacityEffect();
    return tot;
}
