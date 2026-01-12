#ifndef CITY_HPP
#define CITY_HPP

#include <memory>
#include <string>
#include <vector>
#include "Building.hpp"
#include "Street.hpp"
#include "ResourcePool.hpp"

class City {
    std::string name_;
    int money_ = 0;
    ResourcePool<int> resources_;
    std::vector<Street> streets_;
    std::vector<std::shared_ptr<Building>> buildings_;

public:
    explicit City(std::string n, int startingMoney = 0) noexcept;
    City(const City& other);
    City& operator=(City other) noexcept;
    friend void swap(City& a, City& b) noexcept;
    void addStreet(const Street& s);
    Street* getStreet(std::size_t idx);
    [[nodiscard]] const Street* getStreet(std::size_t idx) const;
    void addResource(const std::string& type, int amount);
    void setMoney(int m) noexcept;
    [[nodiscard]] int money() const noexcept;
    void addBuilding(const std::string& typeId, const std::string& name, const std::vector<std::string>& params, std::size_t streetIdx);
    void upgradeAllBuildings();
    void upgradeResidentialOnly();
    [[nodiscard]] int maxBuildings() const noexcept;
    void addBuildingDirect(std::shared_ptr<Building> b);
    [[nodiscard]] int remainingSlots() const noexcept;
    void printSummary() const;
    [[nodiscard]] int totalCapacity() const noexcept;
    ResourcePool<long> producedStats_;
};

#endif // CITY_HPP
