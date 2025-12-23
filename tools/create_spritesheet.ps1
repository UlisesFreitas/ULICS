# Create a simple 128x128 spritesheet with .NET
Add-Type -AssemblyName System.Drawing

$bmp = New-Object System.Drawing.Bitmap(128, 128)
$g = [System.Drawing.Graphics]::FromImage($bmp)

# Fill with black
$g.FillRectangle([System.Drawing.Brushes]::Black, 0, 0, 128, 128)

# Draw simple white squares for player (sprites 0, 1, 2)
$whiteBrush = [System.Drawing.Brushes]::White
$g.FillRectangle($whiteBrush, 1, 2, 6, 5)  # Sprite 0
$g.FillRectangle($whiteBrush, 9, 2, 6, 5)  # Sprite 1  
$g.FillRectangle($whiteBrush, 17, 2, 6, 5) # Sprite 2

# Draw gold circles for coins (sprites 20-23)
$goldBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::Gold)
$g.FillEllipse($goldBrush, 33, 9, 6, 6)  # Sprite 20
$g.FillEllipse($goldBrush, 41, 9, 6, 6)  # Sprite 21
$g.FillEllipse($goldBrush, 49, 9, 6, 6)  # Sprite 22
$g.FillEllipse($goldBrush, 57, 9, 6, 6)  # Sprite 23

# Draw red squares for enemy (sprites 30-31)
$redBrush = [System.Drawing.Brushes]::Red
$g.FillRectangle($redBrush, 113, 9, 6, 6)  # Sprite 30
$g.FillRectangle($redBrush, 121, 9, 6, 6)  # Sprite 31

# Draw orange/yellow triangles for fire (sprites 40-42)  
$orangeBrush = [System.Drawing.Brushes]::Orange
$yellowBrush = [System.Drawing.Brushes]::Yellow
$g.FillRectangle($orangeBrush, 65, 18, 5, 5)  # Sprite 40
$g.FillRectangle($orangeBrush, 73, 17, 5, 6)  # Sprite 41
$g.FillRectangle($yellowBrush, 81, 16, 5, 7)  # Sprite 42

$g.Dispose()
$bmp.Save("$PSScriptRoot\cartridges\test_animations\spritesheet.png", [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()

Write-Host "Created 128x128 spritesheet"
