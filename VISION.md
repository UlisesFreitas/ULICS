# 🎮 ULICS - High-Performance Fantasy Console

## Vision Document

---

## ⚡ DIFERENCIACIÓN CLAVE DE ULICS

```
╔════════════════════════════════════════════════════════╗
║  PICO-8/TIC-80           vs.           ULICS          ║
╠════════════════════════════════════════════════════════╣
║  💾 2MB RAM              →   💾 512MB - 1GB RAM       ║
║  📝 8,192 tokens         →   📝 1,000,000 lines Lua   ║
║  🎨 16 colores fijos     →   🎨 16-128 configurable   ║
║  📦 8KB de código        →   📦 Proyectos masivos     ║
║  🎯 Juegos simples       →   🎯 RPGs, roguelikes, AI  ║
╚════════════════════════════════════════════════════════╝
```

---

## 🎯 FILOSOFÍA DE ULICS

### **"Estética retro + Recursos modernos"**

ULICS es una fantasy console que combina lo mejor de dos mundos: la simplicidad y encanto de las consolas retro con la potencia de hardware moderno.

### Principios Fundamentales

- ✅ **SÍ**: Todo en Lua (sprites como data, no PNGs)
- ✅ **SÍ**: Framebuffer de píxeles indexados
- ✅ **SÍ**: Paleta limitada (pero configurable)
- ✅ **SÍ**: API estilo fantasy console
- ✅ **PERO**: Con memoria MASIVA para proyectos complejos
- ✅ **PERO**: Con código Lua prácticamente ilimitado
- ✅ **PERO**: Sin restricciones artificiales rígidas

---

## 🚀 ¿QUÉ HACE A ULICS ÚNICO?

### 1. **Recursos Masivos con Estética Retro**

ULICS no impone las restricciones de hardware de los 80s. En su lugar, ofrece:

- **512MB - 1GB de RAM**: Para mundos procedurales masivos, sistemas de IA complejos, o miles de entidades simultáneas
- **1,000,000 líneas de Lua**: Para proyectos ambiciosos que requieren código extenso
- **Paletas configurables**: 16, 32, 64, o 128 colores según las necesidades del proyecto
- **Resolución ajustable**: Por defecto 256x256, pero configurable

### 2. **Filosofía "Everything is Code"**

Como PICO-8 y TIC-80, ULICS mantiene la filosofía de que todo está en el código:

```lua
-- Sprites como datos en Lua
local sprites = {
    player = {
        "00111100",
        "01111110",
        "11111111",
        "10111101"
    }
}

-- Mapas como tablas
local world = {
    {1,1,1,1,1},
    {1,0,0,0,1},
    {1,0,2,0,1}
}

-- Música/SFX como datos
local sfx = {
    jump = {freq=440, duration=0.2}
}
```

**Pero** en ULICS, estos datos pueden ser **ENORMES**. Puedes tener:
- Un mundo de 10,000 x 10,000 tiles
- Miles de sprites con docenas de frames de animación
- Sistemas de audio complejos
- Árboles de comportamiento IA con miles de nodos

---

## 💡 CASOS DE USO

### **PICO-8/TIC-80 es ideal para:**
- Juegos de plataformas simples
- Pong, Snake, Tetris
- Mini roguelikes básicos
- Prototipos rápidos
- Game jam entries

### **ULICS es ideal para:**
- ✅ **RPGs completos** con sistemas de quests, inventario, NPCs con diálogos extensos
- ✅ **Roguelikes procedurales** con generación infinita de dungeons
- ✅ **Simuladores** de civilizaciones, ecosistemas, o economías
- ✅ **Motores de física complejos** implementados en Lua
- ✅ **Sistemas de partículas masivos** (miles de partículas simultáneas)
- ✅ **IA procedural avanzada** (pathfinding A*, behavior trees, redes neuronales simples)
- ✅ **Juegos multijugador** con gestión de estado complejo
- ✅ **Herramientas de desarrollo** (editores, generadores, simuladores)

---

## 📊 CONFIGURACIÓN DE CARTUCHOS

### Ejemplo de `config.json` Completo

```json
{
  // === METADATA ===
  "name": "Epic Space RPG",
  "author": "Studio Name",
  "version": "2.1.0",
  "description": "A massive procedurally generated space adventure",
  
  // === LÍMITES DE RECURSOS (CONFIGURABLES) ===
  "memory_limit_mb": 768,           // Entre 512-1024MB
  "lua_code_limit_lines": 500000,   // Hasta 1,000,000
  
  // === CONFIGURACIÓN GRÁFICA ===
  "palette_size": 128,              // 16, 32, 64, 128
  "framebuffer_width": 256,
  "framebuffer_height": 256,
  
  // === OPCIONAL (Futuro) ===
  "target_fps": 60,
  "audio_channels": 8
}
```

### Estructura de Cartucho

```
cartridges/
└── my_epic_rpg/
    ├── main.lua       # Todo el código del juego (puede ser ENORME)
    └── config.json    # Configuración (opcional)
```

**Nota importante**: ULICS NO usa carpetas de assets. Todo (sprites, música, mapas) se define como datos dentro de `main.lua`. Esto mantiene la pureza de fantasy console pero sin las limitaciones de tamaño.

---

## 📝 EJEMPLO PRÁCTICO: Cartucho ULICS vs PICO-8

### PICO-8 (8KB de código)

```lua
-- main.lua
local player = {x=64, y=64}

function _update()
    if btn(0) then player.x-=1 end
    if btn(1) then player.x+=1 end
end

function _draw()
    cls()
    spr(1, player.x, player.y)
end
```

### ULICS (hasta 1M líneas, 768MB RAM)

```lua
-- cartridges/massive_rpg/main.lua

-- ================================================
-- SISTEMA DE TILES MASIVO (10,000 x 10,000)
-- ================================================
local world_tiles = {}
local WORLD_SIZE = 10000

function init_world()
    for y = 1, WORLD_SIZE do
        world_tiles[y] = {}
        for x = 1, WORLD_SIZE do
            -- Generación procedural con noise
            local noise = perlin(x/100, y/100, seed)
            world_tiles[y][x] = determine_tile(noise)
        end
    end
end

-- ================================================
-- SPRITES CON MILES DE FRAMES
-- ================================================
local sprites = {
    player = {
        idle = generate_frames(100),  -- 100 frames de idle
        walk = generate_frames(200),  -- 200 frames de walk
        attack = generate_frames(150) -- 150 frames de attack
    },
    enemies = {}
}

-- Generar 1000 enemigos únicos proceduralmente
for i = 1, 1000 do
    sprites.enemies[i] = generate_enemy_sprite(i, seed)
end

-- ================================================
-- SISTEMA DE QUESTS COMPLEJO
-- ================================================
local quest_database = {}

-- 5000 quests con diálogo, condiciones, rewards
for i = 1, 5000 do
    quest_database[i] = {
        id = i,
        name = generate_quest_name(i),
        description = generate_quest_description(i),
        dialogue = generate_dialogue_tree(i, 50), -- 50 nodos por quest
        conditions = generate_conditions(i),
        rewards = generate_rewards(i),
        npcs = generate_npcs(i, 20) -- 20 NPCs por quest
    }
end

-- ================================================
-- IA PROCEDURAL CON BEHAVIOR TREES
-- ================================================
local ai_behaviors = {
    goblin = create_behavior_tree({
        sequence({
            check_player_in_range(100),
            selector({
                flee_if_low_health(0.3),
                attack_if_close(20),
                patrol()
            })
        })
    }),
    dragon = create_complex_ai(500) -- 500 nodos de decisión
}

-- ================================================
-- SISTEMA DE FÍSICA
-- ================================================
local physics_entities = {}

-- Simular 10,000 partículas simultáneamente
for i = 1, 10000 do
    physics_entities[i] = {
        x = rnd(WORLD_SIZE),
        y = rnd(WORLD_SIZE),
        vx = rnd(10) - 5,
        vy = rnd(10) - 5,
        mass = rnd(100)
    }
end

function update_physics(dt)
    for i = 1, #physics_entities do
        local e = physics_entities[i]
        -- Física compleja (colisiones, gravedad, fricción)
        apply_forces(e)
        integrate_velocity(e, dt)
        handle_collisions(e)
    end
end

-- ================================================
-- GENERACIÓN PROCEDURAL AVANZADA
-- ================================================
function generate_dungeon(depth, size, seed)
    -- Algoritmo BSP (Binary Space Partitioning)
    local tree = bsp_split(0, 0, size, size, depth)
    
    -- Cellular automata para cuevas
    local cave = cellular_automata(size, size, iterations=10)
    
    -- Wave Function Collapse para habitaciones
    local rooms = wfc_generate(patterns, size, size)
    
    return combine_maps(tree, cave, rooms)
end

-- ================================================
-- SISTEMA DE INVENTARIO MASIVO
-- ================================================
local item_database = generate_items(50000) -- 50,000 items únicos

local player_inventory = {
    items = {}, -- Puede contener miles de items
    max_capacity = 10000,
    
    add_item = function(self, item)
        -- Sistema complejo de stacking, sorting, etc.
    end
}

-- ================================================
-- PATHFINDING A* PARA MILES DE ENTIDADES
-- ================================================
local pathfinding_cache = {}

function find_path(from, to, world)
    local key = from.x..","..from.y.."->"..to.x..","..to.y
    
    if pathfinding_cache[key] then
        return pathfinding_cache[key]
    end
    
    -- A* con optimizaciones para mundos masivos
    local path = astar(from, to, world, heuristic=manhattan)
    pathfinding_cache[key] = path
    
    return path
end

-- ================================================
-- SISTEMA DE DIÁLOGO CON MILES DE LÍNEAS
-- ================================================
local dialogue_trees = {}

for npc_id = 1, 1000 do
    dialogue_trees[npc_id] = {
        nodes = generate_dialogue_nodes(100), -- 100 nodos por NPC
        branches = generate_branches(50),     -- 50 ramificaciones
        conditions = generate_conditions(30), -- 30 condiciones
        effects = generate_effects(20)        -- 20 efectos
    }
end

-- ================================================
-- RED NEURONAL SIMPLE PARA IA
-- ================================================
local neural_net = {
    layers = {
        create_layer(128), -- Input
        create_layer(256), -- Hidden 1
        create_layer(256), -- Hidden 2
        create_layer(64)   -- Output
    }
}

function neural_forward_pass(input)
    local output = input
    for i = 1, #neural_net.layers do
        output = activate(neural_net.layers[i], output)
    end
    return output
end

-- ================================================
-- GAME LOOP
-- ================================================
function _update()
    update_physics(dt)
    update_ai_for_all_entities()
    update_quests()
    update_dialogue()
    process_pathfinding()
    update_neural_networks()
end

function _draw()
    -- Renderizar mundo masivo con culling
    local visible_tiles = get_visible_tiles(camera)
    for tile in visible_tiles do
        draw_tile(tile)
    end
    
    draw_entities()
    draw_ui()
    draw_debug_info()
end
```

**Este código sería IMPOSIBLE en PICO-8 (8KB limit).**  
**En ULICS, este es un cartucho NORMAL.**

---

## 🔧 LÍMITES Y ENFORCEMENT

### Filosofía de Límites en ULICS

Los límites en ULICS son **configurables** y **generosos**, no restrictivos:

- **memory_limit_mb**: 512-1024MB (configurable por cartucho)
- **lua_code_limit_lines**: hasta 1,000,000 (configurable)
- **Enforcement**: Soft limits (warnings, no crashes)

```cpp
// Ejemplo de enforcement (informativo, no restrictivo)
void ScriptingManager::CheckResourceLimits() {
    // Memoria Lua
    size_t memory_kb = lua_gc(L, LUA_GCCOUNT, 0);
    size_t memory_mb = memory_kb / 1024;
    
    if (memory_mb > config.memory_limit_mb * 0.9) {
        LOG_WARNING("Memory usage: " + memory_mb + "MB / " 
                    + config.memory_limit_mb + "MB (90%)");
    }
    
    // Mostrar stats, NO crashear
    display_resource_stats();
}
```

### Comportamiento:
- ✅ Mostrar estadísticas de uso en consola
- ✅ Warnings cuando se acerca al límite configurado
- ✅ Límites configurables por cartucho
- ⚠️ **NO crashear** automáticamente (soft limits)
- 📊 Opción de mostrar stats en pantalla durante development

---

## 🎯 COMPARACIÓN TÉCNICA DETALLADA

| Característica | PICO-8 | TIC-80 | **ULICS** |
|----------------|--------|--------|-----------|
| **Memoria RAM** | 2MB | 256KB | **512MB-1GB** ⚡ |
| **Código** | 8,192 tokens | 256KB | **1M líneas Lua** ⚡ |
| **Sprites** | 256 (8x8) | 256 | **Ilimitados (definidos en Lua)** |
| **Mapa** | 128x32 tiles | 240x136 | **Cualquier tamaño (Lua tables)** |
| **Colores** | 16 fijos | 16 | **16-128 configurables** |
| **Resolución** | 128x128 | 240x136 | **256x256 (configurable)** |
| **Canales Audio** | 4 | 4 | **8+ (configurable)** |
| **Pathfinding** | Básico | Básico | **A*, Dijkstra, comportamientos complejos** |
| **IA** | Simple | Simple | **Behavior trees, neural nets** |
| **Física** | Manual | Manual | **Box2D-like en Lua posible** |
| **Saving** | 256 valores | 256 bytes | **Megabytes de save data** |

---

## 🚀 PROYECTOS POSIBLES SOLO EN ULICS

### 1. **Roguelike Masivo Procedural**
- Generación infinita de mazmorras
- Miles de items únicos procedurales
- Sistema de crafting complejo
- IA enemiga con behavior trees
- Sistema de clases y habilidades extenso

### 2. **RPG de Mundo Abierto**
- Mundo de 10,000 x 10,000 tiles
- 1,000+ NPCs con diálogos únicos
- 5,000+ quests procedurales
- Sistema de economía simulado
- Clima y ecosistemas dinámicos

### 3. **Simulador de Civilización**
- Simulación de miles de agentes autónomos
- Economía compleja
- Diplomacia y guerras
- Tecnología procedural
- Generación de historia dinámica

### 4. **Motor de Física Avanzado**
- Simulación de fluidos
- Soft body physics
- Cloth simulation
- Particle systems masivos

### 5. **Plataformas con Generación Procedural**
- Niveles infinitos
- Enemy AI complejo
- Sistema de power-ups extenso
- Ranking y replay system

---

## 📚 DOCUMENTACIÓN DE REFERENCIA

- **LUA_API.md**: API completa de funciones Lua disponibles
- **plan-ulics.json**: Plan de desarrollo por fases
- **README.md**: Guía de inicio rápido

---

## 🎊 RESUMEN EJECUTIVO

### ULICS es:

✅ **Una fantasy console moderna** con estética retro  
✅ **Para proyectos ambiciosos** que PICO-8/TIC-80 no pueden manejar  
✅ **Con recursos masivos** (512MB-1GB, 1M líneas) pero manteniendo simplicidad  
✅ **Sistema de cartuchos** auto-contenidos (todo en Lua)  
✅ **API simple** estilo fantasy console  
✅ **Límites configurables** por proyecto  

### ULICS NO es:

❌ Clon directo de PICO-8  
❌ Engine genérico con assets externos  
❌ Sistema con restricciones artificiales rígidas  
❌ Plataforma para juegos simples exclusivamente  

---

## 💬 FILOSOFÍA FINAL

> **"ULICS te da el lienzo de una fantasy console retro,**  
> **pero con todos los pinceles que necesites para pintar tu obra maestra."**

No limitamos tu creatividad con restricciones arbitrarias de los 80s.  
En ULICS, si puedes imaginarlo en Lua, puedes construirlo.

---

**Versión del documento**: 1.0  
**Fecha**: 2025-12-11  
**Proyecto**: ULICS Fantasy Console
