-- TEST COMPARATIVO - spr() vs anim_play()

function _init()
end

function _update()
end

function _draw()
    cls(1) -- Fondo azul

    -- Test 1: Sprite estático con spr()
    spr(0, 30, 30)
    print("spr(0)", 30, 40, 7)

    -- Test 2: Animación con anim_play()
    anim_play("player_walk", 38, 30)
    print("anim", 38, 40, 7)

    -- Info
    print("TEST COMPARATIVO", 10, 10, 7)
    print("Izq: spr(0,30,30)", 10, 100, 10)
    print("Der: anim_play(38,30)", 10, 110, 10)
end
