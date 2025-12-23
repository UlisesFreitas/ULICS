
// === Helper function to log Lua errors ===
void ScriptingManager::LogError(const std::string& error) {
    if (logFile.is_open()) {
        // Get current time
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t_now));
        
        logFile << "[" << timeStr << "] " << error << std::endl;
        logFile.flush();  // Ensure it's written immediately
    }
}
