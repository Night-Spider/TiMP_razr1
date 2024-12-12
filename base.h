//base.h
#pragma once
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>

class base {
public:
    base(const std::string& filename);
    void load();
    const std::map<std::string, std::string>& get_users() const;

private:
    std::string filename_;
    std::map<std::string, std::string> users_;
};
