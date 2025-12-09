#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Street;
class Building {
protected:
    std::string name_;
    int level_;
    int maxLevel_;
    static int buildingCount_;
    virtual void printImpl(std::ostream& os) const = 0;

public:
    explicit Building(std::string name = "Building", int lvl = 1, int maxL = 3);
    virtual ~Building();
    Building(const Building&) = default;
    Building& operator=(const Building&) = default;
    void print(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const Building& b);
    //functii virtuale
    virtual void upgrade(std::map<std::string,int>& cityResources, int& money) = 0;
    [[nodiscard]] virtual std::shared_ptr<Building> clone_shared() const = 0;
    [[nodiscard]] virtual int capacityEffect() const = 0;
    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] int level() const noexcept;
    [[nodiscard]] static int buildingCount() noexcept;
};

class BuildingCreator {
public:
    using Creator = std::function<std::shared_ptr<Building>(const std::string&, const std::vector<std::string>&, Street*)>;

private:
    std::map<std::string, Creator> registry_;
    mutable std::mutex mtx_;

public:
    static BuildingCreator& instance();
    void registerCreator(const std::string& id, Creator c);

    std::shared_ptr<Building> create(const std::string& id, const std::string& name, const std::vector<std::string>& params, Street* street) const;
};
//clase derivate
class ResidentialBuilding : public Building {
    int capacityBase_;
    std::map<std::string,int> resourcesNeeded_;
    int moneyProducedPerUpgrade_;
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override;

public:
    ResidentialBuilding(const std::string& n, int cap, int lvl, const std::map<std::string,int>& resNeeded, int moneyPerUpgrade, Street* st);

    void upgrade(std::map<std::string,int>& cityResources, int& money) override;
    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override;
    [[nodiscard]] int capacityEffect() const override;
};

class UtilityBuilding : public Building {
    double coverage_;
    int moneyCostPerUpgrade_;
    std::string type_;
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override;

public:
    UtilityBuilding(const std::string& n, std::string t, double cov, int lvl, int moneyCost, Street* st);
    void upgrade(std::map<std::string,int>& cityResources, int& money) override;
    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override;
    [[nodiscard]] int capacityEffect() const override;
};

class Park : public Building {
    double populationBoost_;
    int moneyCost_;
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override;

public:
    Park(const std::string& n, double boost, int cost, Street* st);
    void upgrade(std::map<std::string,int>& cityResources, int& money) override;
    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override;
    [[nodiscard]] int capacityEffect() const override;
    [[nodiscard]] int cost() const noexcept;
};

class CommercialBuilding : public Building {
    int customersPerLevel_;
    Street* street_ = nullptr;

protected:
    void printImpl(std::ostream& os) const override;

public:
    CommercialBuilding(const std::string& n, int baseCustomers, int lvl, Street* st);

    void upgrade(std::map<std::string,int>& cityResources, int& money) override;
    [[nodiscard]] std::shared_ptr<Building> clone_shared() const override;
    [[nodiscard]] int capacityEffect() const override;
};

class Slot {
    std::shared_ptr<Building> building_;
public:
    explicit Slot(std::shared_ptr<Building> b = nullptr) noexcept;

    void setBuilding(std::shared_ptr<Building> b) noexcept;

    void upgradeSlot(std::map<std::string,int>& resources, int& money) const;
    void show(std::ostream& os) const;

    [[nodiscard]] int capacity() const noexcept;
    [[nodiscard]] std::shared_ptr<Building> building() const noexcept;
};

#endif // BUILDING_HPP
