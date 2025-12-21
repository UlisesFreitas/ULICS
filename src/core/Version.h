#ifndef VERSION_H
#define VERSION_H

/**
 * @brief ULICS Engine Version Information
 * 
 * Semantic Versioning: MAJOR.MINOR.PATCH
 * - MAJOR: Breaking changes
 * - MINOR: New features (backwards compatible)
 * - PATCH: Bug fixes
 */

namespace ULICS {
    // Version Constants
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 1;
    constexpr int VERSION_PATCH = 0;

    // Version String
    constexpr const char* VERSION_STRING = "1.1.0";
    constexpr const char* VERSION_NAME = "WTF!";

    // Full version string with name
    constexpr const char* VERSION_FULL = "ULICS v1.1.0 - WTF!";

    // Build Information
    constexpr const char* BUILD_DATE = __DATE__;
    constexpr const char* BUILD_TIME = __TIME__;

    // Engine Name
    constexpr const char* ENGINE_NAME = "ULICS Fantasy Console";

} // namespace ULICS

#endif // VERSION_H
