-- ULICS API Test Suite
-- Systematically tests all implemented API functions

local test_idx = 1
local test_results = {}
local current_test = ""
local tests = {
    "Graphics", "Input", "Math", "System"
}
local test_stage = 1
local frame_counter = 0

-- Forward declarations
local pass, fail, run_all_tests
local test_graphics_api, test_input_api, test_math_api, test_system_api

-- Test result tracking
pass = function(name)
    test_results[test_idx] = { name = name, status = "PASS" }
    test_idx = test_idx + 1
end

fail = function(name, error_msg)
    test_results[test_idx] = { name = name, status = "FAIL", error = error_msg }
    test_idx = test_idx + 1
end

function _init()
    current_test = "Initializing API Test Suite"
end

function _update()
    frame_counter = frame_counter + 1

    -- Run tests only once after a few frames
    if frame_counter == 10 then
        run_all_tests()
    end

    -- Navigate test stages with arrow keys
    if btnp(2) and test_stage > 1 then
        test_stage = test_stage - 1
    end
    if btnp(3) and test_stage < 4 then
        test_stage = test_stage + 1
    end

    -- Exit with Z
    if btnp(4) then
        goto_menu()
    end
end

run_all_tests = function()
    current_test = "Running tests..."

    -- === GRAPHICS API TESTS ===
    test_graphics_api()

    -- === INPUT API TESTS ===
    test_input_api()

    -- === MATH API TESTS ===
    test_math_api()

    -- === SYSTEM API TESTS ===
    test_system_api()

    current_test = "Tests Complete"
end

test_graphics_api = function()
    -- Test cls/clear
    local ok, err = pcall(function() cls(0) end)
    if ok then pass("cls()") else fail("cls()", err) end

    -- Test pset
    ok, err = pcall(function() pset(10, 10, 8) end)
    if ok then pass("pset()") else fail("pset()", err) end

    -- Test line
    ok, err = pcall(function() line(0, 0, 20, 20, 7) end)
    if ok then pass("line()") else fail("line()", err) end

    -- Test rect
    ok, err = pcall(function() rect(5, 5, 30, 30, 11) end)
    if ok then pass("rect()") else fail("rect()", err) end

    -- Test rectfill
    ok, err = pcall(function() rectfill(40, 5, 20, 20, 9) end)
    if ok then pass("rectfill()") else fail("rectfill()", err) end

    -- Test circ
    ok, err = pcall(function() circ(100, 50, 15, 12) end)
    if ok then pass("circ()") else fail("circ()", err) end

    -- Test circfill
    ok, err = pcall(function() circfill(150, 50, 10, 14) end)
    if ok then pass("circfill()") else fail("circfill()", err) end

    -- Test pget
    ok, err = pcall(function() local c = pget(10, 10) end)
    if ok then pass("pget()") else fail("pget()", err) end

    -- Test print
    ok, err = pcall(function() print("TEST", 10, 100, 7) end)
    if ok then pass("print()") else fail("print()", err) end

    -- Test camera
    ok, err = pcall(function() camera(0, 0) end)
    if ok then pass("camera()") else fail("camera()", err) end

    -- Test tcolor
    ok, err = pcall(function() tcolor(0) end)
    if ok then pass("tcolor()") else fail("tcolor()", err) end
end

test_input_api = function()
    -- Test btn
    local ok, err = pcall(function() local b = btn(0) end)
    if ok then pass("btn()") else fail("btn()", err) end

    -- Test btnp
    ok, err = pcall(function() local b = btnp(0) end)
    if ok then pass("btnp()") else fail("btnp()", err) end
end

test_math_api = function()
    -- Test sin
    local ok, err = pcall(function() local s = sin(0.25) end)
    if ok then pass("sin()") else fail("sin()", err) end

    -- Test cos
    ok, err = pcall(function() local c = cos(0.5) end)
    if ok then pass("cos()") else fail("cos()", err) end

    -- Test atan2
    ok, err = pcall(function() local a = atan2(1, 1) end)
    if ok then pass("atan2()") else fail("atan2()", err) end

    -- Test sqrt
    ok, err = pcall(function() local s = sqrt(16) end)
    if ok then pass("sqrt()") else fail("sqrt()", err) end

    -- Test abs
    ok, err = pcall(function() local a = abs(-5) end)
    if ok then pass("abs()") else fail("abs()", err) end

    -- Test flr
    ok, err = pcall(function() local f = flr(3.7) end)
    if ok then pass("flr()") else fail("flr()", err) end

    -- Test ceil
    ok, err = pcall(function() local c = ceil(3.2) end)
    if ok then pass("ceil()") else fail("ceil()", err) end

    -- Test rnd
    ok, err = pcall(function() local r = rnd(10) end)
    if ok then pass("rnd()") else fail("rnd()", err) end
end

test_system_api = function()
    -- Test time
    local ok, err = pcall(function() local t = time() end)
    if ok then pass("time()") else fail("time()", err) end

    -- Test list_cartridges (don't call, just check it exists)
    ok, err = pcall(function()
        if list_cartridges then
            return true
        else
            error("not defined")
        end
    end)
    if ok then pass("list_cartridges()") else fail("list_cartridges()", err) end

    -- Test exit/reset/goto_menu (check existence only)
    if exit then pass("exit()") else fail("exit()", "not defined") end
    if reset then pass("reset()") else fail("reset()", "not defined") end
    if goto_menu then pass("goto_menu()") else fail("goto_menu()", "not defined") end
end

function _draw()
    cls(0) -- Black background

    -- Header
    print("ULICS API TEST SUITE", 50, 5, 7)
    line(0, 15, 255, 15, 7)

    -- Current status
    print("Status: " .. current_test, 5, 20, 6)
    print("Tests: " .. #test_results, 5, 30, 6)

    -- Display tests for current category
    local y = 45
    local tests_per_page = 16
    local start_idx = 1
    local end_idx = #test_results

    -- Filter by category
    if test_stage == 1 then
        print("=== GRAPHICS TESTS ===", 60, 40, 11)
        start_idx = 1
        end_idx = 11
    elseif test_stage == 2 then
        print("=== INPUT TESTS ===", 70, 40, 11)
        start_idx = 12
        end_idx = 13
    elseif test_stage == 3 then
        print("=== MATH TESTS ===", 72, 40, 11)
        start_idx = 14
        end_idx = 21
    elseif test_stage == 4 then
        print("=== SYSTEM TESTS ===", 65, 40, 11)
        start_idx = 22
        end_idx = #test_results
    end

    y = 50
    for i = start_idx, end_idx do
        if test_results[i] and y < 220 then
            local color = test_results[i].status == "PASS" and 11 or 8
            local symbol = test_results[i].status == "PASS" and "+" or "X"
            print(symbol .. " " .. test_results[i].name, 10, y, color)
            y = y + 8
        end
    end

    -- Footer
    line(0, 225, 255, 225, 7)
    print("UP/DN:NAVIGATE  Z:MENU", 45, 235, 14)

    -- Stats
    local pass_count = 0
    local fail_count = 0
    for i = 1, #test_results do
        if test_results[i].status == "PASS" then
            pass_count = pass_count + 1
        else
            fail_count = fail_count + 1
        end
    end

    print("PASS:" .. pass_count, 10, 245, 11)
    print("FAIL:" .. fail_count, 180, 245, 8)
end

-- Initialize
_init()