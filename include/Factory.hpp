#pragma once

#include <map>
#include <memory>
#include <ranges>
#include <string>

#include "Building.hpp"
#include "Street.hpp"
#include "Exceptions.hpp"
#include "BuildingVisitor.hpp"

class FactoryBuilding : public Building {
    std::map<std::string,int> production_;
    int costPerProduction_;
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override {
        os << "Factory(name=" << name_ << ", production={";
        bool first = true;
        for (const auto& kv : production_) {
            if (!first) os << ", ";
            os << kv.first << ":" << kv.second;
            first = false;
        }
        os << "}, cost=" << costPerProduction_ << ")";
        if (street_) {
            os << " [street level=" << street_->level()
               << ", segments=" << street_->length() << "]";
        }
    }

public:
    FactoryBuilding(const std::string& n,
                    const std::map<std::string,int>& prod,
                    int cost,
                    Street* st)
        : Building(n, 1, 1), production_(prod), costPerProduction_(cost), street_(st)
    {
        if (production_.empty())
            throw CityException("Factory must produce at least one resource");
        if (costPerProduction_ <= 0)
            throw CityException("Factory must have a positive production cost");
    }

    void accept(BuildingVisitor& v) override;

    void upgrade(ResourcePool<int>&, int&) override {
    }

    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override {
        return std::make_shared<FactoryBuilding>(*this);
    }

    [[nodiscard]] int capacityEffect() const override {
        int total = 0;
        for (const auto &val: production_ | std::views::values) total += val;
        return total;
    }

    void produce(ResourcePool<int>& cityResources, int& money, ResourcePool<long>& stats) const {
        if (!trySpend(money, costPerProduction_))
            throw CityException("Not enough money to activate factory production");

        for (const auto& kv : production_) {
            cityResources.add(kv.first, kv.second);          // stoc curent (int)
            stats.add(kv.first, static_cast<long>(kv.second)); // total produs (long)
        }
    }
};

inline void FactoryBuilding::accept(BuildingVisitor& v) { v.visit(*this); }
