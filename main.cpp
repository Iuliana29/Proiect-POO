#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "include/City.hpp"
#include "include/Exceptions.hpp"
#include "include/ResourcePool.hpp"

int main() {
    try {
        std::ifstream fin("tastatura.txt");
        if (!fin) {
            std::cerr << "Cannot open tastatura.txt\n";
            return 1;
        }

        std::string tag;
        std::string cityName;
        int cityMoney = 0;

        fin >> tag;
        if (tag != "CITY") throw CityException("Missing CITY section");
        fin >> cityName >> cityMoney;

        City city(cityName, cityMoney);

        {
            int adminTax = 10;
            if (!trySpend(cityMoney, adminTax))
                throw CityException("Not enough money for initial tax");
            city.setMoney(cityMoney);
        }

        int streetCount = 0;
        fin >> tag;
        if (tag != "STREETS") throw CityException("Missing STREETS section");
        fin >> streetCount;

        for (int i = 0; i < streetCount; ++i) {
            fin >> tag;
            if (tag != "STREET") throw CityException("Was waiting for STREET");

            int streetId = 0, segmentCount = 0;
            fin >> streetId >> segmentCount;

            Street s(streetId);

            for (int j = 0; j < segmentCount; ++j) {
                int seg = 0;
                fin >> seg;
                s.addSegment(seg);
            }

            city.addStreet(s);
        }

        int resourceCount = 0;
        fin >> tag;
        if (tag != "RESOURCES") throw CityException("Missing RESOURCES section");
        fin >> resourceCount;

        for (int i = 0; i < resourceCount; ++i) {
            fin >> tag;
            if (tag != "RESOURCE") throw CityException("Was waiting for RESOURCE");

            std::string resName;
            int qty = 0;
            fin >> resName >> qty;
            city.addResource(resName, qty);
        }

        int buildingCount = 0;
        fin >> tag;
        if (tag != "BUILDINGS") throw CityException("Missing BUILDINGS section");
        fin >> buildingCount;

        for (int i = 0; i < buildingCount; ++i) {
            fin >> tag;
            if (tag != "BUILDING") throw CityException("Was waiting for BUILDING");

            std::string type, name;
            int streetIndex = 0;
            int paramCount = 0;

            fin >> type >> name >> streetIndex >> paramCount;

            std::vector<std::string> params(paramCount);
            for (int j = 0; j < paramCount; ++j) fin >> params[j];

            city.addBuilding(type, name, params, static_cast<std::size_t>(streetIndex));
        }

        city.addBuilding("factory", "WoodFactory", {"wood", "15", "30"}, 0);

        std::cout << "--- INITIAL CITY STATE ---\n";
        city.printSummary();

        std::cout << "\n--- Upgrading all buildings (Visitor tick) ---\n";
        city.upgradeAllBuildings(); // producedStats_ se actualizeaza

        std::cout << "\n--- Upgrading residential buildings only ---\n";
        city.upgradeResidentialOnly();

        // scor/raportare cheltuieli cumulative
        {
            long reportBudget = 5000;
            long reportCost = 200L;
            // cheltuim dintr-un buget
            (void)trySpend(reportBudget, reportCost);
        }

        std::cout << "\n--- CITY STATE AFTER UPGRADES ---\n";
        city.printSummary();
        std::cout << "Total capacity: " << city.totalCapacity() << "\n";
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
