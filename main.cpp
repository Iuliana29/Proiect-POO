#include <iostream>
#include <map>
#include <string>
#include "City.hpp"
#include "Building.hpp"
#include "Exceptions.hpp"

int main() {
    try {
        City city("RefactoredCity", 100);
        Street s1(2);
        s1.addSegment(1);
        s1.addSegment(2);
        city.addStreet(s1);
        Street s2(1);
        s2.addSegment(1);
        city.addStreet(s2);
        city.addResource("wood", 50);
        city.addResource("stone", 30);
        city.addBuilding("residential", "Home", {"10","1","20"}, 0);
        city.addBuilding("utility", "WaterPlant", {"Water","100","1","50"}, 0);
        city.addBuilding("park", "CentralPark", {"15.0","30"}, 0);
        city.addBuilding("commercial", "Mall", {"60","1"}, 1);
        std::cout << "--- INITIAL CITY STATE ---\n";
        city.printSummary();
        std::cout << "\n--- Upgrading all buildings (polymorphic calls) ---\n";
        city.upgradeAllBuildings();
        std::cout << "\n--- Upgrading residential buildings only ---\n";
        city.upgradeResidentialOnly();
        std::cout << "\n--- CITY STATE AFTER UPGRADES ---\n";
        city.printSummary();
        std::cout << "Total capacity: " << city.totalCapacity() << "\n";
        std::map<std::string,int> extraRes{{"wood",5},{"stone",2}};
        int extraMoney = 50;
        Block b1(std::make_shared<ResidentialBuilding>(
            "BlockHome", 5, 1,
            std::map<std::string,int>{{"wood",5},{"stone",2}},
            10, city.getStreet(0)));
        std::cout << "\nBlock1 info: ";
        b1.show(std::cout);
        std::cout << "\nBlock1 capacity: " << b1.capacity() << "\n";
        b1.upgradeBlock(extraRes, extraMoney);
        std::cout << "After upgrade, Block1 capacity: " << b1.capacity()
                  << ", extraMoney=" << extraMoney << "\n";
        Block b2;
        b2.setBuilding(b1.building());
        std::cout << "\nBlock2 info (copied from Block1): ";
        b2.show(std::cout);
        std::cout << "\nBlock2 capacity: " << b2.capacity() << "\n";
        auto extraBuilding = std::make_shared<ResidentialBuilding>(
            "ExtraHome", 3, 1,
            std::map<std::string,int>{{"wood",5},{"stone",2}},
            5, city.getStreet(0));
        city.addBuildingDirect(extraBuilding);
        std::cout << "\n--- CITY STATE AFTER addBuildingDirect ---\n";
        city.printSummary();
        std::cout << "Total capacity after ExtraHome: "
                  << city.totalCapacity() << "\n";
        City other = city;
        other.setMoney(500);
        std::cout << "\n--- COPIED CITY ---\n";
        other.printSummary();
    }
    catch (const CityException& e) {
        std::cout << "City error: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << "Std exception: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
