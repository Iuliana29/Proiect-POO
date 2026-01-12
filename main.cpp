#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "include/City.hpp"
#include "include/Building.hpp"
#include "include/Factory.hpp"
#include "include/Exceptions.hpp"
#include "include/ResourcePool.hpp"

int main() {
    try {
        std::ifstream fin("tastatura.txt");
        if (!fin) {
            std::cerr << "Nu pot deschide fisierul tastatura.txt\n";
            return 1;
        }

        std::string tag;
        std::string cityName;
        int cityMoney = 0;

        fin >> tag;
        if (tag != "CITY") throw CityException("Missing section CITY");
        fin >> cityName >> cityMoney;

        City city(cityName, cityMoney);

        {
            int m = city.money();
            constexpr int adminTax = 10;
            if (!trySpend(m, adminTax))
                throw CityException("Not enough money for tax");
            city.setMoney(m);
        }

        int streetCount = 0;
        fin >> tag;
        if (tag != "STREETS") throw CityException("Missing section STREETS");
        fin >> streetCount;

        for (int i = 0; i < streetCount; ++i) {
            fin >> tag;
            if (tag != "STREET") throw CityException("Was expecting STREET");

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
        if (tag != "RESOURCES") throw CityException("Missing section RESOURCES");
        fin >> resourceCount;

        for (int i = 0; i < resourceCount; ++i) {
            fin >> tag;
            if (tag != "RESOURCE") throw CityException("Was expecting RESOURCE");

            std::string resName;
            int qty = 0;
            fin >> resName >> qty;
            city.addResource(resName, qty);
        }

        int buildingCount = 0;
        fin >> tag;
        if (tag != "BUILDINGS") throw CityException("Missing section BUILDINGS");
        fin >> buildingCount;

        for (int i = 0; i < buildingCount; ++i) {
            fin >> tag;
            if (tag != "BUILDING") throw CityException("Was expecting BUILDING");

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

        std::cout << "\n--- DISTRICT PLANNING ---\n";
        ResourcePool<int> districtMaterials;
        districtMaterials.add("wood", 30);
        districtMaterials.add("stone", 20);

        std::vector<Slot> district;

        district.emplace_back(std::make_shared<ResidentialBuilding>(
            "BlockA", 5, 1,
            std::map<std::string,int>{{"wood", 5}, {"stone", 2}},
            10,
            city.getStreet(0)
        ));

        district.emplace_back(std::make_shared<CommercialBuilding>(
            "ShopA", 30, 1, city.getStreet(0)
        ));

        district.emplace_back();
        district[2].setBuilding(std::make_shared<Park>(
            "PocketPark", 5.0, 0, city.getStreet(0)
        ));

        int localMoney = city.money();
        for (const auto& slot : district) {
            std::cout << "Parcel: ";
            slot.show(std::cout);
            std::cout << " | capacity=" << slot.capacity() << "\n";

            if (slot.building()) {
                slot.upgradeSlot(districtMaterials, localMoney);
            }
        }
        city.setMoney(localMoney);

        //  adaugam cladirile in oras
        std::cout << "\n--- COMMIT DISTRICT INTO CITY ---\n";
        for (const auto& slot : district) {
            if (slot.building()) {
                // clonam ca sa nu partajam pointerul districtului cu orasul
                city.addBuildingDirect(slot.building()->clone_shared());
            }
        }
        city.upgradeAllBuildings();
        city.upgradeResidentialOnly();

        ResourcePool<long> ledger;
        ledger.add("tax_collected", 100L);
        ledger.add("maintenance_paid", 40L);

        long reportBudget = 5000;
        (void)trySpend(reportBudget, 200L);

        std::cout << "\n--- CITY STATE AFTER GAMEPLAY ---\n";
        city.printSummary();
        std::cout << "Total capacity: " << city.totalCapacity() << "\n";
        std::cout << "Ledger tax_collected=" << ledger.get("tax_collected")
                  << ", maintenance_paid=" << ledger.get("maintenance_paid") << "\n";
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
