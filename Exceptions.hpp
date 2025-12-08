#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

class CityException : public std::exception {
protected:
    std::string msg_;
public:
    explicit CityException(std::string m);
    [[nodiscard]] const char* what() const noexcept override;
};

class InvalidIndexException : public CityException {
public:
    InvalidIndexException();
};

class LimitExceededException : public CityException {
public:
    LimitExceededException();
};

class InsufficientResourceException : public CityException {
public:
    explicit InsufficientResourceException(const std::string& r);
};

#endif // EXCEPTIONS_HPP
