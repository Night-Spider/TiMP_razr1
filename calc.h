//calc.h
#pragma once
#include <vector>
#include <cstdint>
#include <limits>

class calc {
public:
    calc(const std::vector<uint32_t>& vector);
    uint32_t send_res();

private:
    std::vector<uint32_t> vector_;
};
