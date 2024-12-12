//calc.cpp
#include "calc.h"
#include <vector>
#include <limits>

calc::calc(const std::vector<uint32_t>& vector) : vector_(vector) {}

uint32_t calc::send_res() {
    uint32_t result = 1;
    for (const auto& value : vector_) {
        if (result > std::numeric_limits<uint32_t>::max() / value) {
            return std::numeric_limits<uint32_t>::max(); // Переполнение
        }
        result *= value;
    }
    return result;
}
