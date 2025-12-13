# clean.ps1 - Complete clean rebuild script
# Use this when you want to rebuild EVERYTHING from scratch

Write-Host "=== ULICS Complete Clean Rebuild ===" -ForegroundColor Cyan
Write-Host ""

# Remove all generated directories
Write-Host "Removing build artifacts..." -ForegroundColor Yellow
Remove-Item -Recurse -Force build, sdl, external -ErrorAction SilentlyContinue

Write-Host ""
Write-Host "All build artifacts removed." -ForegroundColor Green
Write-Host "Now running build.ps1 to rebuild from scratch..." -ForegroundColor Cyan
Write-Host ""

# Run the build script
.\build.ps1
