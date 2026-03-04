#pragma once

#include <cstdint>
#include <cstring>
#include <string_view>
#include <ostream>

//abstracción de string de no-copia y no-asignación, rapida para mover "ventanas" a través del codigo fuente.

struct Str
{

public:

    constexpr Str() = default;

    template<std::size_t N>
    constexpr Str(const char (&lit)[N]) : p(reinterpret_cast<const uint8_t*>(lit)) {
        len = N-1;
    }

    constexpr Str(const uint8_t* start, std::size_t lenght) : p(start), len(lenght) {}

    uint8_t operator[](std::size_t i) const {
        return p[i];
    }

    bool operator==(const Str& other) const {
        if(len != other.len) return false;
        if(p == other.p) return true;
        return std::memcmp(p, other.p, len) == 0;
    }

    ptrdiff_t operator-(const Str& other) const {
        return p - other.getPtr();
    }

    friend std::ostream& operator<<(std::ostream &os, Str& s){
        return os << s.view();
    }

    std::string_view view() const {
        return std::string_view(reinterpret_cast<const char*>(p), len);
    }

    std::size_t getLen() const noexcept {
        return len;
    }

    const uint8_t* getPtr() const noexcept {
        return p;
    }

private:
    const uint8_t *p = nullptr;
    std::size_t len = 0;

public:

};
