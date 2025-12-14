# build-correct.ps1 - CORRECT build script that works
# Use this instead of build.ps1 until we fix it

Write-Host "=== ULICS Build (Working Method) ===" -ForegroundColor Cyan
Write-Host ""

# Step 1: Clean build directory
Write-Host "Cleaning build directory..." -ForegroundColor Yellow
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

# Step 2: Configure with CMake
Write-Host "Configuring project..." -ForegroundColor Yellow


# Step 3: Build Release
Write-Host ""
Write-Host "Building Release..." -ForegroundColor Yellow
cmake --build build --config Release

Write-Host ""
Write-Host "=== Build Complete ===" -ForegroundColor Green
Write-Host "Executable: .\build\bin\Release\UliCS.exe" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run: .\build\bin\Release\UliCS.exe" -ForegroundColor White
