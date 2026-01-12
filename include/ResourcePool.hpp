#pragma once
#include <map>
#include <string>
#include "Exceptions.hpp"

template <typename T>
class ResourcePool {
    std::map<std::string, T> data_;

public:
    void add(const std::string& name, T qty) {
        if (qty < 0) throw CityException("Negative add not allowed");
        data_[name] += qty;
    }

    T get(const std::string& name) const {
        auto it = data_.find(name);
        return (it == data_.end()) ? T{} : it->second;
    }

    bool canConsume(const std::string& name, T qty) const {
        return get(name) >= qty;
    }

    void consume(const std::string& name, T qty) {
        auto cur = get(name);
        if (cur < qty) throw InsufficientResourceException(name);
        data_[name] = cur - qty;
    }

    const std::map<std::string, T>& raw() const noexcept { return data_; }
};

template <typename T>
bool trySpend(T& money, T cost) {
    if (money < cost) return false;
    money -= cost;
    return true;
}
