#ifndef LUA_GAME_H
#define LUA_GAME_H

#include "game/Game.h"

// Forward declarations
struct lua_State;
class ScriptingManager;

/// @class LuaGame
/// @brief An implementation of the Game interface that delegates its logic to a Lua script.
/// This class acts as a bridge between the C++ engine and the Lua environment.
class LuaGame : public Game {
public:
    explicit LuaGame(ScriptingManager* scriptingManager);
    ~LuaGame() override = default;

    bool _update() override;
    void _draw(AestheticLayer& aestheticLayer) override;

private:
    ScriptingManager* scriptingManager; // Non-owning pointer to the manager.
};

#endif // LUA_GAME_H