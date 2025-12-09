#include "Factory.hpp"


namespace {

    [[maybe_unused]] const bool factory_registered = [](){
        BuildingCreator::instance().registerCreator(
            "factory",
            [](const std::string& name,
               const std::vector<std::string>& params,
               Street* st) -> std::shared_ptr<Building>
            {
                // parametrii:
                // [0] = nume resursa
                // [1] = cantitate
                // [2] = cost per productie
                std::string resName = !params.empty() ? params[0]:"wood";
                int amount = params.size() > 1 ? std::stoi(params[1]) : 5;
                int cost = params.size() > 2 ? std::stoi(params[2]) : 20;
                std::map<std::string,int> prod{{resName, amount}};
                return std::make_shared<FactoryBuilding>(name, prod, cost, st);
            }
        );
        return true;
    }();

}
