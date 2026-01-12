#ifndef STREET_HPP
#define STREET_HPP

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

constexpr std::size_t MAX_SEGMENTS = 10;

class Street {
    std::vector<int> segments_;
    int level_ = 1;
public:
    explicit Street(int lvl = 1) noexcept;
    bool addSegment(int seg);
    [[nodiscard]] int length() const noexcept;
    [[nodiscard]] int level() const noexcept;
    [[nodiscard]] std::string roadType() const;
    friend std::ostream& operator<<(std::ostream& os, const Street& s);
};

#endif // STREET_HPP
