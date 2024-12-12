//error.h
#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

class crit_err : public std::runtime_error {
public:
    explicit crit_err(const std::string& message) : std::runtime_error(message) {}
};

class no_crit_err : public std::runtime_error {
public:
    explicit no_crit_err(const std::string& message) : std::runtime_error(message) {}
};

#endif // ERROR_H
