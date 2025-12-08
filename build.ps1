# build.ps1 - Script to configure and build the ULICS project.
# This script automates the CMake configuration and build process, and logs all output.

# --- Terminate running application instance ---
$processName = "UliCS"
$runningProcess = Get-Process -Name $processName -ErrorAction SilentlyContinue
if ($runningProcess) {
    Write-Host "An instance of '$($processName).exe' is currently running. Terminating it before build..."
    Stop-Process -Name $processName -Force
    Write-Host "Process terminated successfully."
}

# Get the directory where the script is located to ensure paths are correct.
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$LogFile = Join-Path -Path $ScriptDir -ChildPath "build_log.txt"

# Clean up old log and start new transcript to capture all output
if (Test-Path $LogFile) {
    Remove-Item $LogFile
}
Start-Transcript -Path $LogFile

$BuildDir = Join-Path -Path $ScriptDir -ChildPath "build"
$SdlDir = Join-Path -Path $ScriptDir -ChildPath "sdl"
$ErrorActionPreference = "Stop"

try {
    # --- Step 1: Ensure SDL2 source code is available ---
    if (-not (Test-Path -Path $SdlDir)) {
        Write-Host "SDL2 source not found. Cloning from repository..."
        # Clone only the latest version (--depth 1) for a faster and smaller download.
        git clone --depth 1 -b SDL2 https://github.com/libsdl-org/SDL.git $SdlDir
        # Remove the .git folder from the cloned SDL directory.
        # This prevents VS Code from detecting it as a nested repository.
        Remove-Item -Path (Join-Path $SdlDir ".git") -Recurse -Force
    } else {
        Write-Host "SDL2 source found."
    }

    # --- Step 2: Clean and recreate the build directory ---
    Write-Host "Performing a clean build. Removing and recreating build directory..."
    if (Test-Path -Path $BuildDir -PathType Container) {
        Remove-Item -Path $BuildDir -Recurse -Force
    }
    New-Item -ItemType Directory -Path $BuildDir | Out-Null

    # --- Step 3: Run CMake to Configure ---
    Write-Host "[1/2] Configuring project with CMake..."
    
    Push-Location -Path $BuildDir
    # Configure CMake. This will also configure the SDL build because of 'add_subdirectory'.
    # We specify the generator to be sure. The options for static linking are now in CMakeLists.txt.
    cmake .. -G "Visual Studio 16 2019"
    Pop-Location
    
    Write-Host "CMake configuration successful." -ForegroundColor Green

    # --- Step 4: Build the Project ---
    Write-Host "[2/2] Building project via CMake..."
    # Build the 'Release' configuration for a smaller, optimized executable.
    cmake --build $BuildDir --config Release
    
    Write-Host "Project built successfully!" -ForegroundColor Green
    Write-Host "You can find the executable at: $($BuildDir)\bin\Release\UliCS.exe"

} catch {
    Write-Error "An error occurred during the build process: $_"
    # Exit with a non-zero code to indicate failure
    exit 1
} finally {
    # Ensure the transcript is always stopped.
    Stop-Transcript
}
