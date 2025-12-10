// tests/CartridgeLoader_test.cpp

#include "gtest/gtest.h"
#include "cartridge/CartridgeLoader.h"
#include <filesystem>
#include <fstream>
#include <chrono> // For timing measurements

// Test fixture for CartridgeLoader tests.
// This fixture handles the creation and cleanup of temporary cartridge files.
class CartridgeLoaderTest : public ::testing::Test {
protected:
    // Create a temporary directory for our test cartridges.
    const std::filesystem::path testDir = std::filesystem::temp_directory_path() / "ulics_tests";

    void SetUp() override {
        // Ensure the directory is clean before each test.
        std::filesystem::remove_all(testDir);
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        // Clean up the directory after each test.
        std::filesystem::remove_all(testDir);
    }

    // Helper function to create a dummy cartridge on the fly.
    void CreateDummyCartridge(const std::string& name, const std::string& config, const std::string& script) {
        std::filesystem::path cartDir = testDir / name;
        std::filesystem::create_directory(cartDir);

        std::ofstream configFile(cartDir / "config.json");
        configFile << config;

        std::ofstream scriptFile(cartDir / "main.lua");
        scriptFile << script;
    }
};

// Test case to verify successful loading of a valid cartridge.
TEST_F(CartridgeLoaderTest, SuccessfullyLoadsValidCartridge) {
    // 1. Arrange: Create a valid dummy cartridge.
    const std::string dummyConfig = R"({"title": "Test Game", "author": "Tester"})";
    const std::string dummyScript = "function _update() print('hello') end";
    CreateDummyCartridge("test_game", dummyConfig, dummyScript);

    // 2. Act: Attempt to load the cartridge.
    auto start = std::chrono::high_resolution_clock::now();

    auto cartridge = CartridgeLoader::loadCartridge((testDir / "test_game").string());

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    // Print the duration to the console during the test run.
    std::cout << "[   INFO   ] Cartridge loading took " << duration.count() << " ms." << std::endl;

    // 3. Assert: Check that loading was successful and the cartridge object is valid.
    ASSERT_NE(cartridge, nullptr);
    EXPECT_EQ(cartridge->config["title"], "Test Game");
    EXPECT_EQ(cartridge->luaScript, dummyScript);
}

// Test case to verify it fails to load a cartridge with a missing script.
TEST_F(CartridgeLoaderTest, FailsToLoadCartridgeWithMissingScript) {
    // 1. Arrange: Create a cartridge with a config but no main.lua.
    CreateDummyCartridge("broken_game", R"({"title": "Broken"})", "");
    std::filesystem::remove(testDir / "broken_game" / "main.lua");

    // 2. Act & 3. Assert: Attempt to load and expect failure.
    auto cartridge = CartridgeLoader::loadCartridge((testDir / "broken_game").string());
    EXPECT_EQ(cartridge, nullptr);
}

// Test case for the static scanForCartridges function.
TEST_F(CartridgeLoaderTest, ScanForCartridgesFindsCorrectCarts) {
    // 1. Arrange: Create multiple dummy cartridges.
    CreateDummyCartridge("game1", R"({"title": "Game One"})", "print('1')");
    CreateDummyCartridge("game2", R"({"title": "Game Two"})", "print('2')");
    // Create a directory without a config.json, which should be ignored.
    std::filesystem::create_directory(testDir / "not_a_game");
    // Create a hidden directory, which should be ignored.
    CreateDummyCartridge(".hidden_game", R"({"title": "Hidden"})", "print('h')");

    // 2. Act: Scan the directory.
    auto foundCarts = CartridgeLoader::scanForCartridges(testDir.string());

    // 3. Assert: Check that exactly two valid cartridges were found.
    ASSERT_EQ(foundCarts.size(), 2);
    EXPECT_EQ(foundCarts[0].title, "Game One");
    EXPECT_EQ(foundCarts[1].title, "Game Two");
}