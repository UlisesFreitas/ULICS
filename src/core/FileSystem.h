#ifndef ULICS_FILESYSTEM_H
#define ULICS_FILESYSTEM_H

#include <string>

namespace Ulics {
namespace FileSystem {

/**
 * @brief Gets the platform-specific application data directory.
 * 
 * Uses SDL_GetPrefPath to find a suitable location for writing user data.
 * e.g., on Windows: C:/Users/YourUser/AppData/Roaming/com.ulics.dev/ULICS/
 * @param orgName The organization name.
 * @param appName The application name.
 * @return The full path to the user data directory, including a trailing slash.
 *         Returns an empty string on failure.
 */
std::string getUserDataPath(const char* orgName, const char* appName);

} // namespace FileSystem
} // namespace Ulics

#endif // ULICS_FILESYSTEM_H