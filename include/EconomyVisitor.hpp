#pragma once
#include "BuildingVisitor.hpp"
#include "ResourcePool.hpp"
#include "Factory.hpp"

class EconomyTickVisitor : public BuildingVisitor {
    ResourcePool<int>& res_;
    int& money_;
    ResourcePool<long>& stats_;

public:
    EconomyTickVisitor(ResourcePool<int>& r, int& m, ResourcePool<long>& s)
        : res_(r), money_(m), stats_(s) {}

    void visit(ResidentialBuilding& b) override { b.upgrade(res_, money_); }
    void visit(UtilityBuilding& b) override     { b.upgrade(res_, money_); }
    void visit(Park& b) override                { b.upgrade(res_, money_); }
    void visit(CommercialBuilding& b) override  { b.upgrade(res_, money_); }
    void visit(FactoryBuilding& b) override     { b.produce(res_, money_, stats_); }
};
