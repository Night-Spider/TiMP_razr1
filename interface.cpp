//interface.cpp
#include "interface.h"
#include <iostream>
#include <cstring>

bool interface::parser(int argc, const char** argv) {
    basefile_ = "base.txt"; // Значение по умолчанию для файла базы
    logfile_ = "log.txt"; // Значение по умолчанию для файла лога
    port_ = 33333; // Значение по умолчанию для порта

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0) {
            std::cout << "Usage: vcalc [options]\n"
                      << "-b <basefile>  Specify the user database file\n"
                      << "-l <logfile>   Specify the log file\n"
                      << "-p <port>      Specify the port number\n"
                      << "-h             Show this help message\n";
            return false;
        } else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            basefile_ = argv[++i];
        } else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            logfile_ = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port_ = std::stoi(argv[++i]);
        }
    }
    return true;
}

std::string interface::get_base() const {
    return basefile_;
}

std::string interface::get_log() const {
    return logfile_;
}

uint32_t interface::get_port() const {
    return port_;
}

void interface::setup_connection() {
    logger log(logfile_); // Инициализация логгера
    try {
        log.writelog("Настройка соединения с базой: " + basefile_);
        base user_base(basefile_); // Создаем объект базы
        user_base.load(); // Загружаем базу пользователей
        log.writelog("База пользователей успешно загружена");
    } catch (const std::runtime_error& e) { // Используем std::runtime_error вместо crit_err
        log.writelog("Ошибка: " + std::string(e.what()));
        throw; // Пробрасываем исключение дальше
    } catch (const std::exception& e) {
        log.writelog("Ошибка: " + std::string(e.what()));
        throw; // Пробрасываем исключение дальше
    }
}
