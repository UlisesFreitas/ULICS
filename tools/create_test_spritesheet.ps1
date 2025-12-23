Add-Type -AssemblyName System.Drawing

# Create 128x128 spritesheet
$width = 128
$height = 128
$bitmap = New-Object System.Drawing.Bitmap($width, $height)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)

# Fill with black background
$graphics.Clear([System.Drawing.Color]::Black)

# Sprite 0 (0,0) - Red square
$red = [System.Drawing.Color]::FromArgb(255, 0, 0)
$brush = New-Object System.Drawing.SolidBrush($red)
$graphics.FillRectangle($brush, 1, 1, 6, 6)

# Sprite 1 (8,0) - Green square
$green = [System.Drawing.Color]::FromArgb(0, 255, 0)
$brush = New-Object System.Drawing.SolidBrush($green)
$graphics.FillRectangle($brush, 9, 1, 6, 6)

# Sprite 2 (16,0) - Blue square
$blue = [System.Drawing.Color]::FromArgb(0, 0, 255)
$brush = New-Object System.Drawing.SolidBrush($blue)
$graphics.FillRectangle($brush, 17, 1, 6, 6)

# Save
$outputPath = ".\cartridges\test_animations\spritesheet.png"
$bitmap.Save($outputPath, [System.Drawing.Imaging.ImageFormat]::Png)

Write-Host "Spritesheet created: $outputPath"
Write-Host "Sprite 0: RED square"
Write-Host "Sprite 1: GREEN square"
Write-Host "Sprite 2: BLUE square"

$graphics.Dispose()
$bitmap.Dispose()
