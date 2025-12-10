// tests/GameLoader_test.cpp

#include "gtest/gtest.h"
#include "core/Engine.h"
#include "cartridge/GameLoader.h"
#include "scripting/LuaGame.h"
#include <filesystem>
#include <fstream>

// Test fixture for GameLoader tests.
// This fixture handles the creation of a headless engine and temporary cartridge files.
class GameLoaderTest : public ::testing::Test {
protected:
    std::unique_ptr<Engine> engine;
    const std::filesystem::path testDir = std::filesystem::temp_directory_path() / "ulics_gl_tests";

    void SetUp() override {
        // 1. Create a clean environment for each test.
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir / "cartridges");

        // 2. Create a headless Engine instance for testing.
        // We pass a special flag to the constructor to prevent it from creating a window.
        engine = std::make_unique<Engine>(/*headless=*/true);
        
        // 3. Manually initialize the engine and override the user data path to our temp directory.
        engine->InitializeHeadless(testDir.string());
    }

    void TearDown() override {
        // Clean up the directory after each test.
        std::filesystem::remove_all(testDir);
        engine.reset();
    }

    // Helper function to create a dummy cartridge on the fly.
    void CreateDummyCartridge(const std::string& name, const std::string& config, const std::string& script) {
        std::filesystem::path cartDir = testDir / "cartridges" / name;
        std::filesystem::create_directory(cartDir);

        std::ofstream configFile(cartDir / "config.json");
        configFile << config;

        std::ofstream scriptFile(cartDir / "main.lua");
        scriptFile << script;
    }
};

// Test case to verify successful loading of a valid cartridge.
TEST_F(GameLoaderTest, SuccessfullyLoadsValidCartridge) {
    // 1. Arrange: Create a valid dummy cartridge.
    const std::string dummyConfig = R"({"title": "Loader Test Game"})";
    const std::string dummyScript = "function _init() end function _update() end function _draw() end";
    CreateDummyCartridge("loader_test", dummyConfig, dummyScript);

    // 2. Act: Attempt to load the game synchronously.
    // We pass a null progress pointer as we are not testing the progress reporting here.
    auto game = GameLoader::loadAndInitializeGame(engine.get(), "loader_test", nullptr);

    // 3. Assert: Check that a valid LuaGame object was created.
    ASSERT_NE(game, nullptr);
    EXPECT_EQ(game->getConfig()["title"], "Loader Test Game");
}

// Test case to verify it fails to load a non-existent cartridge.
TEST_F(GameLoaderTest, FailsToLoadNonExistentCartridge) {
    // 1. Arrange: No cartridge is created.

    // 2. Act: Attempt to load a game that doesn't exist.
    auto game = GameLoader::loadAndInitializeGame(engine.get(), "non_existent_game", nullptr);

    // 3. Assert: Check that the result is a nullptr.
    EXPECT_EQ(game, nullptr);
}