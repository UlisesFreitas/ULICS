# Create a colored test spritesheet for test_animations
Add-Type -AssemblyName System.Drawing

$width = 128
$height = 128
$bitmap = New-Object System.Drawing.Bitmap($width, $height)

# Define colors (PICO-8 palette)
$colors = @(
    [System.Drawing.Color]::FromArgb(0, 0, 0),        # 0: Black
    [System.Drawing.Color]::FromArgb(255, 0, 0),      # 1: Red
    [System.Drawing.Color]::FromArgb(0, 255, 0),      # 2: Green
    [System.Drawing.Color]::FromArgb(255, 255, 0),    # 3: Yellow
    [System.Drawing.Color]::FromArgb(0, 0, 255),      # 4: Blue
    [System.Drawing.Color]::FromArgb(255, 0, 255),    # 5: Magenta
    [System.Drawing.Color]::FromArgb(0, 255, 255),    # 6: Cyan
    [System.Drawing.Color]::FromArgb(255, 255, 255)   # 7: White
)

# Fill with transparent background first
for ($y = 0; $y -lt $height; $y++) {
    for ($x = 0; $x -lt $width; $x++) {
        $bitmap.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
    }
}

# Create sprite 0: Red square with yellow center (frame 1 of walk)
for ($y = 0; $y -lt 8; $y++) {
    for ($x = 0; $x -lt 8; $x++) {
        if ($x -ge 2 -and $x -le 5 -and $y -ge 2 -and $y -le 5) {
            $bitmap.SetPixel($x, $y, $colors[3])  # Yellow center
        }
        else {
            $bitmap.SetPixel($x, $y, $colors[1])  # Red border
        }
    }
}

# Create sprite 1: Green square with cyan center (frame 2 of walk)
for ($y = 0; $y -lt 8; $y++) {
    for ($x = 0; $x -lt 8; $x++) {
        if ($x -ge 2 -and $x -le 5 -and $y -ge 2 -and $y -le 5) {
            $bitmap.SetPixel($x + 8, $y, $colors[6])  # Cyan center
        }
        else {
            $bitmap.SetPixel($x + 8, $y, $colors[2])  # Green border
        }
    }
}

# Create sprite 2: Blue square with magenta center (frame 3 of walk)
for ($y = 0; $y -lt 8; $y++) {
    for ($x = 0; $x -lt 8; $x++) {
        if ($x -ge 2 -and $x -le 5 -and $y -ge 2 -and $y -le 5) {
            $bitmap.SetPixel($x + 16, $y, $colors[5])  # Magenta center
        }
        else {
            $bitmap.SetPixel($x + 16, $y, $colors[4])  # Blue border
        }
    }
}

# Save
$outputPath = ".\cartridges\test_animations\spritesheet.png"
$bitmap.Save($outputPath, [System.Drawing.Imaging.ImageFormat]::Png)
Write-Host "Created colored spritesheet: $outputPath"
$bitmap.Dispose()
