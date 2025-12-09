#pragma once

#include <map>
#include <memory>
#include <ranges>
#include <string>
#include "Building.hpp"
#include "Street.hpp"
#include "Exceptions.hpp"

// O fabrica produce resurse in schimbul banilor
class FactoryBuilding : public Building {
    std::map<std::string,int> production_;  // ce resurse produce
    int costPerProduction_;                 // cat costa o runda de productie
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override {
        os << "Factory(name=" << name_ << ", production={";

        bool first = true;
        for (auto& kv : production_) {
            if (!first) os << ", ";
            os << kv.first << ":" << kv.second;
            first = false;
        }

        os << "}, cost=" << costPerProduction_ << ")";

        if (street_) {
            os << " [street level=" << street_->level() << ", segments=" << street_->length() << "]";
        }
    }

public:
    FactoryBuilding(const std::string& n, const std::map<std::string,int>& prod, int cost, Street* st)
        : Building(n, 1, 1),production_(prod), costPerProduction_(cost), street_(st)
    {
        if (production_.empty())
            throw CityException("Factory must produce at least one resource");
        if (costPerProduction_ <= 0)
            throw CityException("Factory must have a positive production cost");
    }

    // o fabrica nu se upgrade-aza
    void upgrade(std::map<std::string,int>&, int&) override {

    }

    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override {
        return std::make_shared<FactoryBuilding>(*this);
    }

    //  capacitatea de productie totala
    [[nodiscard]] int capacityEffect() const override {
        int total = 0;
        for (const auto &val: production_ | std::views::values) total += val;
        return total;
    }

    void produce(std::map<std::string,int>& cityResources, int& money) const {
        if (money < costPerProduction_)
            throw CityException("Not enough money to activate factory production");

        money -= costPerProduction_;

        for (const auto& kv : production_)
            cityResources[kv.first] += kv.second;
    }
};
