//base.cpp
#include "base.h"
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>

base::base(const std::string& filename) : filename_(filename) {}

void base::load() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка открытия файла базы пользователей: " + filename_);
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string id = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            users_[id] = password;
        }
    }
    file.close();
}

const std::map<std::string, std::string>& base::get_users() const {
    return users_;
}
