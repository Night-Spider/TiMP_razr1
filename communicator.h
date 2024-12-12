//communicator.h
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>
#include <map>
#include "log.h"

class communicator {
public:
    static std::string sha256(const std::string& input_str);
    int connection(uint32_t port, const std::map<std::string, std::string>& base, logger* l);
    uint64_t generate_salt(); // Генерация соли как 64-битного числа
    std::string salt_to_hex(uint64_t salt); // Преобразование соли в шестнадцатеричный формат
};

#endif // COMMUNICATOR_H
