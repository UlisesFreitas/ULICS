# ulics.ps1 - ULICS Command Line Tool
# Quick access to ULICS tools

param(
    [Parameter(Mandatory = $false, Position = 0)]
    [string]$Command,
    
    [Parameter(Mandatory = $false, Position = 1)]
    [string]$Name,
    
    [Parameter(ValueFromRemainingArguments = $true)]
    $RemainingArgs
)

function Show-Help {
    Write-Host "ULICS Command Line Tool" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage:" -ForegroundColor Yellow
    Write-Host "  .\ulics.ps1 new <name> [options]    Create a new cartridge" -ForegroundColor White
    Write-Host "  .\ulics.ps1 build                    Build ULICS" -ForegroundColor White
    Write-Host "  .\ulics.ps1 run                      Run ULICS" -ForegroundColor White
    Write-Host "  .\ulics.ps1 help                     Show this help" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Yellow
    Write-Host "  .\ulics.ps1 new my_game" -ForegroundColor Gray
    Write-Host "  .\ulics.ps1 new my_platformer -Template Platformer" -ForegroundColor Gray
    Write-Host "  .\ulics.ps1 new puzzle_game -Template Puzzle -Author 'Your Name'" -ForegroundColor Gray
    Write-Host ""
}

switch ($Command) {
    "new" {
        if ([string]::IsNullOrEmpty($Name)) {
            Write-Host "ERROR: Please provide a cartridge name" -ForegroundColor Red
            Write-Host "Usage: .\ulics.ps1 new <name>" -ForegroundColor Yellow
            exit 1
        }
        & ".\tools\new-cartridge.ps1" -Name $Name @RemainingArgs
    }
    
    "build" {
        Write-Host "Building ULICS..." -ForegroundColor Cyan
        & ".\build-correct.ps1"
    }
    
    "run" {
        Write-Host "Running ULICS..." -ForegroundColor Cyan
        & ".\build\bin\Release\UliCS.exe"
    }
    
    "help" {
        Show-Help
    }
    
    default {
        if ([string]::IsNullOrEmpty($Command)) {
            Show-Help
        }
        else {
            Write-Host "Unknown command: $Command" -ForegroundColor Red
            Write-Host ""
            Show-Help
        }
    }
}
