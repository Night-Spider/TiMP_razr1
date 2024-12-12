//interface.h
#pragma once
#include <string>
#include <cstdint>
#include <stdexcept>
#include "base.h" // Подключаем заголовок базового класса
#include "log.h"  // Подключаем заголовок для логирования

class interface {
public:
    bool parser(int argc, const char** argv);
    std::string get_base() const;
    std::string get_log() const;
    uint32_t get_port() const; // Метод для получения порта
    void setup_connection(); // Метод для настройки соединения

private:
    std::string basefile_;
    std::string logfile_;
    uint32_t port_;
};
