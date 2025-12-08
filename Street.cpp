#include "Street.hpp"
#include <algorithm>

Street::Street(int lvl) noexcept
    : level_(std::max(1, std::min(3, lvl))) {}

bool Street::addSegment(int seg) {
    if (segments_.size() >= MAX_SEGMENTS) return false;
    segments_.push_back(seg);
    (void)seg;
    return true;
}

int Street::length() const noexcept {
    return static_cast<int>(segments_.size());
}

int Street::level() const noexcept {
    return level_;
}

std::string Street::roadType() const {
    switch (level_) {
        case 1: return "Two lane";
        case 2: return "Four lane";
        case 3: return "Six lane";
        default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, const Street& s) {
    os << "Street(segments=" << s.segments_.size()<< ", " << s.roadType() << ")";
    return os;
}
