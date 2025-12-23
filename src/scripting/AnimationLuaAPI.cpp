
// ============================================
// ANIMATION API (PICO-8/TIC-80 style)
// ============================================

/**
 * @brief anim_play(name, x, y, [flip_x], [flip_y])
 * 
 * All-in-one: Start animation if not playing and draw current frame
 * 
 * @param name (string) Animation name
 * @param x (int) Screen X position
 * @param y (int) Screen Y position
 * @param flip_x (bool, optional) Flip horizontally
 * @param flip_y (bool, optional) Flip vertically
 */
int ScriptingManager::Lua_AnimPlay(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();
    
    if (!animMgr || !layer) {
        return 0;
    }
    
    // Get parameters
    const char* name = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    bool flip_x = lua_toboolean(L, 4);
    bool flip_y = lua_toboolean(L, 5);
    
    // Start animation if not playing
    if (!animMgr->IsPlaying(name)) {
        animMgr->Play(name);
    }
    
    // Get current sprite ID and draw it
    int spriteId = animMgr->GetCurrentSpriteId(name);
    if (spriteId != -1) {
        layer->Spr(spriteId, x, y, flip_x, flip_y);
    }
    
    return 0;
}

/**
 * @brief anim_draw(name, x, y, [flip_x], [flip_y])
 * 
 * Draw current frame without controlling animation state
 */
int ScriptingManager::Lua_AnimDraw(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    AestheticLayer* layer = sm->engineInstance->getAestheticLayer();
    
    if (!animMgr || !layer) {
        return 0;
    }
    
    const char* name = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    bool flip_x = lua_toboolean(L, 4);
    bool flip_y = lua_toboolean(L, 5);
    
    int spriteId = animMgr->GetCurrentSpriteId(name);
    if (spriteId != -1) {
        layer->Spr(spriteId, x, y, flip_x, flip_y);
    }
    
    return 0;
}

/**
 * @brief anim_start(name)
 * 
 * Start/resume animation
 */
int ScriptingManager::Lua_AnimStart(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        return 0;
    }
    
    const char* name = luaL_checkstring(L, 1);
    animMgr->Play(name);
    
    return 0;
}

/**
 * @brief anim_stop(name)
 * 
 * Stop animation and reset to frame 0
 */
int ScriptingManager::Lua_AnimStop(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        return 0;
    }
    
    const char* name = luaL_checkstring(L, 1);
    animMgr->Stop(name);
    
    return 0;
}

/**
 * @brief anim_pause(name)
 * 
 * Pause animation at current frame
 */
int ScriptingManager::Lua_AnimPause(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        return 0;
    }
    
    const char* name = luaL_checkstring(L, 1);
    animMgr->Pause(name);
    
    return 0;
}

/**
 * @brief anim_reset(name)
 * 
 * Reset animation to frame 0 without stopping
 */
int ScriptingManager::Lua_AnimReset(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        return 0;
    }
    
    const char* name = luaL_checkstring(L, 1);
    animMgr->Reset(name);
    
    return 0;
}

/**
 * @brief anim_get_frame(name) → int
 * 
 * Get current sprite ID for animation
 * 
 * @return Sprite ID (0-255) or -1 if not found
 */
int ScriptingManager::Lua_AnimGetFrame(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushinteger(L, -1);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    int spriteId = animMgr->GetCurrentSpriteId(name);
    
    lua_pushinteger(L, spriteId);
    return 1;
}

/**
 * @brief anim_is_playing(name) → bool
 * 
 * Check if animation is currently playing
 */
int ScriptingManager::Lua_AnimIsPlaying(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    bool playing = animMgr->IsPlaying(name);
    
    lua_pushboolean(L, playing);
    return 1;
}

/**
 * @brief anim_is_finished(name) → bool
 * 
 * Check if non-loop animation has finished
 */
int ScriptingManager::Lua_AnimIsFinished(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    bool finished = animMgr->IsFinished(name);
    
    lua_pushboolean(L, finished);
    return 1;
}

/**
 * @brief anim_exists(name) → bool
 * 
 * Check if animation is loaded
 */
int ScriptingManager::Lua_AnimExists(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    bool exists = animMgr->Exists(name);
    
    lua_pushboolean(L, exists);
    return 1;
}

/**
 * @brief anim_get_length(name) → int
 * 
 * Get total number of frames in animation
 */
int ScriptingManager::Lua_AnimGetLength(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    int length = animMgr->GetFrameCount(name);
    
    lua_pushinteger(L, length);
    return 1;
}

/**
 * @brief anim_get_duration(name) → int
 * 
 * Get total duration in engine frames @60fps
 */
int ScriptingManager::Lua_AnimGetDuration(lua_State* L) {
    auto* sm = static_cast<ScriptingManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    AnimationManager* animMgr = sm->engineInstance->getAnimationManager();
    
    if (!animMgr) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    const char* name = luaL_checkstring(L, 1);
    int duration = animMgr->GetTotalDuration(name);
    
    lua_pushinteger(L, duration);
    return 1;
}
