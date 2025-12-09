#include "core/FileSystem.h"
#include <SDL.h>
#include <memory>
#include <iostream>

namespace Ulics {
namespace FileSystem {

std::string getUserDataPath(const char* orgName, const char* appName) {
    char* sdlPath = SDL_GetPrefPath(orgName, appName);
    if (!sdlPath) {
        std::cerr << "FileSystem Error: Unable to get preference path from SDL: " << SDL_GetError() << std::endl;
        return "";
    }
    
    std::string path(sdlPath);
    SDL_free(sdlPath); // Important: free the memory allocated by SDL.
    return path;
}

} // namespace FileSystem
} // namespace Ulics