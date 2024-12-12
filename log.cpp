//log.cpp
#include "log.h"
#include "error.h"
#include <fstream>
#include <stdexcept>
#include <ctime>
#include <sys/stat.h>

logger::logger() : path_to_logfile("log.txt") {}  // Установим значение по умолчанию
logger::logger(const std::string& path) : path_to_logfile(path) {}

int logger::set_path(const std::string& path_file) {
    if (path_file.empty() || path_file.find('.') == std::string::npos) {
        throw crit_err("Неверное расширение файла лога.");
    }
    
    // Проверка существования директории
    struct stat info;
    std::string directory = path_file.substr(0, path_file.find_last_of('/'));
    if (stat(directory.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        throw crit_err("Указанный путь к директории не существует.");
    }
    
    path_to_logfile = path_file;
    return 0;
}

int logger::writelog(const std::string& message) {
    if (path_to_logfile.empty()) {
        throw crit_err("Путь к файлу лога не задан.");
    }

    std::ofstream logfile(path_to_logfile, std::ios_base::out | std::ios_base::app);
    if (!logfile.is_open()) {
        throw crit_err("Ошибка открытия файла лога.");
    }

    logfile << gettime() << ' ' << message << std::endl;
    return 0;
}

std::string logger::gettime() {
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%F %X", timeinfo);
    return std::string(buffer);
}

std::string logger::get_path() const {
    return path_to_logfile;
}

