#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "City.hpp"
#include "Building.hpp"
#include "Exceptions.hpp"

int main() {
    try {
        std::ifstream fin("tastatura.txt");
        if (!fin) {
            std::cerr << "Nu pot deschide fisierul tastatura.txt\n";
            return 1;
        }

        std::string tag;
        std::string cityName;
        int cityMoney;

        fin >> tag;
        if (tag != "CITY") {
            throw CityException("Fisier invalid: lipseste sectiunea CITY");
        }
        fin >> cityName >> cityMoney;

        City city(cityName, cityMoney);
        int streetCount;
        fin >> tag;
        if (tag != "STREETS") {
            throw CityException("Fisier invalid: lipseste sectiunea STREETS");
        }
        fin >> streetCount;            // nr strazi

        for (int i = 0; i < streetCount; ++i) {
            fin >> tag;
            if (tag != "STREET") {
                throw CityException("Fisier invalid: se astepta STREET");
            }

            int streetId, segmentCount;
            fin >> streetId >> segmentCount;  // id strada + nr segmente

            Street s(streetId);

            // citim segmentele
            for (int j = 0; j < segmentCount; ++j) {
                int seg;
                fin >> seg;
                s.addSegment(seg);
            }

            city.addStreet(s);
        }

        int resourceCount;
        fin >> tag;
        if (tag != "RESOURCES") {
            throw CityException("Fisier invalid: lipseste sectiunea RESOURCES");
        }
        fin >> resourceCount;

        for (int i = 0; i < resourceCount; ++i) {
            fin >> tag;
            if (tag != "RESOURCE") {
                throw CityException("Fisier invalid: se astepta RESOURCE");
            }

            std::string resName;
            int qty;
            fin >> resName >> qty;     // nume + cantitate
            city.addResource(resName, qty);
        }

        int buildingCount;
        fin >> tag;
        if (tag != "BUILDINGS") {
            throw CityException("Fisier invalid: lipseste sectiunea BUILDINGS");
        }
        fin >> buildingCount;

        for (int i = 0; i < buildingCount; ++i) {
            fin >> tag;
            if (tag != "BUILDING") {
                throw CityException("Fisier invalid: se astepta BUILDING");
            }

            std::string type, name;
            int streetIndex;
            int paramCount;

            // citire tip, nume, strada si nr parametri
            fin >> type >> name >> streetIndex >> paramCount;

            // citire parametri ca string-uri
            std::vector<std::string> params(paramCount);
            for (int j = 0; j < paramCount; ++j) {
                fin >> params[j];
            }

            city.addBuilding(type, name, params, streetIndex);
        }

        std::cout << "--- INITIAL CITY STATE ---\n";
        city.printSummary();

        std::cout << "\n--- Upgrading all buildings (polymorphic calls) ---\n";
        city.upgradeAllBuildings();

        std::cout << "\n--- Upgrading residential buildings only ---\n";
        city.upgradeResidentialOnly();

        std::cout << "\n--- CITY STATE AFTER UPGRADES ---\n";
        city.printSummary();
        std::cout << "Total capacity: " << city.totalCapacity() << "\n";

        // testare Slot / upgrade
        std::map<std::string,int> extraRes{{"wood",5},{"stone",2}};
        int extraMoney = 50;

        // Slot 1 cu o cladire rezidentiala
        Slot sl1(std::make_shared<ResidentialBuilding>(
            "SlotHome", 5, 1,
            std::map<std::string,int>{{"wood",5},{"stone",2}},
            10, city.getStreet(0)));

        std::cout << "\nSlot1 info: ";
        sl1.show(std::cout);
        std::cout << "\nSlot1 capacity: " << sl1.capacity() << "\n";

        sl1.upgradeSlot(extraRes, extraMoney);
        std::cout << "After upgrade, Slot1 capacity: " << sl1.capacity()
                  << ", extraMoney=" << extraMoney << "\n";

        // Slot 2 copiaza cladirea din Slot 1
        Slot sl2;
        sl2.setBuilding(sl1.building());

        std::cout << "\nSlot2 info (copied from Slot1): ";
        sl2.show(std::cout);
        std::cout << "\nSlot2 capacity: " << sl2.capacity() << "\n";

        // cladire adaugata direct
        auto extraBuilding = std::make_shared<ResidentialBuilding>(
            "ExtraHome", 3, 1,
            std::map<std::string,int>{{"wood",5},{"stone",2}},
            5, city.getStreet(0));

        city.addBuildingDirect(extraBuilding);

        std::cout << "\n--- CITY STATE AFTER addBuildingDirect ---\n";
        city.printSummary();
        std::cout << "Total capacity after ExtraHome: " << city.totalCapacity() << "\n";

        // testare copiere oras
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
