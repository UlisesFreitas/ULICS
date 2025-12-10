#ifndef LUA_GAME_H
#define LUA_GAME_H

#include "game/Game.h"
#include "cartridge/Cartridge.h"
#include "scripting/ScriptingManager.h"
#include <memory>
#include <nlohmann/json.hpp>

// Forward declarations
struct lua_State;

/// @class LuaGame
/// @brief An implementation of the Game interface that delegates its logic to a Lua script.
/// This class is a self-contained unit, owning the script and its environment.
class LuaGame : public Game {
public:
    explicit LuaGame(std::unique_ptr<Cartridge> cart, std::unique_ptr<ScriptingManager> manager);
    ~LuaGame() override = default;

    bool _update() override;
    void _draw(AestheticLayer& aestheticLayer) override;

    const nlohmann::json& getConfig() const;

private:
    std::unique_ptr<Cartridge> cartridge;
    std::unique_ptr<ScriptingManager> scriptingManager;
};

#endif // LUA_GAME_H