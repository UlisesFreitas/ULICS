#ifndef VERSION_H
#define VERSION_H

/**
 * @file Version.h
 * @brief ULICS Engine Version Information
 * 
 * Semantic versioning: MAJOR.MINOR.PATCH
 * - MAJOR: Incompatible API changes
 * - MINOR: Backwards-compatible functionality additions
 * - PATCH: Backwards-compatible bug fixes
 */

namespace ULICS {

// Version Constants
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

// Version String
constexpr const char* VERSION_STRING = "1.0.0";
constexpr const char* VERSION_NAME = "First Light";

// Build Information
constexpr const char* BUILD_DATE = __DATE__;
constexpr const char* BUILD_TIME = __TIME__;

// Engine Name
constexpr const char* ENGINE_NAME = "ULICS Fantasy Console";
constexpr const char* ENGINE_FULL_NAME = "ULICS v1.0.0 - First Light";

} // namespace ULICS

#endif // VERSION_H
