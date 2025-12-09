#ifndef ULICS_CONSTANTS_H
#define ULICS_CONSTANTS_H

#include <string_view>

namespace Ulics {
namespace Constants {

// --- Application Info ---
// Used for window titles and preference paths.
constexpr std::string_view APP_NAME = "ULICS";
constexpr std::string_view ORGANIZATION_NAME = "com.ulics.dev";
constexpr std::string_view APP_DESCRIPTION = "Fantasy Console Superpowered.";

// --- Version Info ---
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_REVISION = 0;

} // namespace Constants
} // namespace Ulics

#endif // ULICS_CONSTANTS_H