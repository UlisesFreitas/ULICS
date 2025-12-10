#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <string>
#include "nlohmann/json.hpp"

// Represents the loaded data of a single game cartridge.
struct Cartridge {
    nlohmann::json config;
    std::string luaScript;
};

#endif // CARTRIDGE_H
