#include "Exceptions.hpp"

CityException::CityException(std::string m) : msg_(std::move(m)) {}

const char* CityException::what() const noexcept {
    return msg_.c_str();
}

InvalidIndexException::InvalidIndexException()
    : CityException("Invalid index provided.") {}

LimitExceededException::LimitExceededException()
    : CityException("Limit exceeded (buildings/streets/resources).") {}

InsufficientResourceException::InsufficientResourceException(const std::string& r)
    : CityException("Insufficient resource: " + r) {}
