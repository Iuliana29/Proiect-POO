#include "Street.hpp"
#include <algorithm>

// seteaza nivelul strazii în intervalul [1,3]
Street::Street(int lvl) noexcept
    : level_(std::max(1, std::min(3, lvl))) {}

bool Street::addSegment(int seg) {
    // nu adaugam daca am atins limita
    if (segments_.size() >= MAX_SEGMENTS) return false;

    // adaugam segmentul in vector
    segments_.push_back(seg);
    (void)seg; // evita warning daca seg nu e folosit în debug
    return true;
}

// lungimea strazii = numarul de segmente
int Street::length() const noexcept {
    return static_cast<int>(segments_.size());
}

// returneaza nivelul strazii (1–3)
int Street::level() const noexcept {
    return level_;
}

// tipul drumului in functie de nivel
std::string Street::roadType() const {
    switch (level_) {
        case 1: return "Two lane";
        case 2: return "Four lane";
        case 3: return "Six lane";
        default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, const Street& s) {
    os << "Street(segments=" << s.segments_.size() << ", " << s.roadType() << ")";
    return os;
}
