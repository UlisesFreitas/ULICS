# Create proper 128x128 spritesheet for test_animations
Add-Type -AssemblyName System.Drawing

$bmp = New-Object System.Drawing.Bitmap(128, 128)
$g = [System.Drawing.Graphics]::FromImage($bmp)
$g.Clear([System.Drawing.Color]::Black)

# Helper function to draw at grid position
function Draw-At($col, $row, $drawAction) {
    $x = $col * 8
    $y = $row * 8
    & $drawAction $x $y
}

# PLAYER WALK ANIMATION (sprites 0, 1, 2) - Simple stick figure
# Sprite 0: Standing
Draw-At 0 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 1, 2, 3)  # Head
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 4, 2, 3)  # Body
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 7, 4, 1)  # Legs together
}

# Sprite 1: Walking - left leg forward
Draw-At 1 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 1, 2, 3)  # Head
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 4, 2, 3)  # Body
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 7, 2, 1)  # Left leg
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 4, $y + 6, 1, 1)  # Right leg back
}

# Sprite 2: Walking - right leg forward  
Draw-At 2 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 1, 2, 3)  # Head
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 4, 2, 3)  # Body
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 4, $y + 7, 2, 1)  # Right leg
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 6, 1, 1)  # Left leg back
}

# PLAYER JUMP ANIMATION (sprites 10, 11, 12)
# Sprite 10: Crouch
Draw-At 10 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 3, 4, 2)  # Head lower
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 5, 4, 2)  # Body compressed
}

# Sprite 11: Jump - legs tucked
Draw-At 11 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 1, 2, 2)  # Head
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 3, 4, 3)  # Body + legs tucked
}

# Sprite 12: Landing
Draw-At 12 0 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 2, 2, 2)  # Head
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 3, $y + 4, 2, 2)  # Body
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 1, $y + 6, 6, 1)  # Wide stance
}

# COIN SPIN ANIMATION (sprites 20, 21, 22, 23)
$gold = [System.Drawing.Color]::FromArgb(255, 215, 0)
$goldBrush = New-Object System.Drawing.SolidBrush($gold)

# Sprite 20: Full circle (front view)
Draw-At 4 1 {
    param($x, $y)
    $g.FillEllipse($goldBrush, $x + 2, $y + 2, 4, 4)
    $g.DrawEllipse([System.Drawing.Pens]::Black, $x + 2, $y + 2, 4, 4)
}

# Sprite 21: Oval (turning)
Draw-At 5 1 {
    param($x, $y)
    $g.FillEllipse($goldBrush, $x + 3, $y + 2, 2, 4)
    $g.DrawEllipse([System.Drawing.Pens]::Black, $x + 3, $y + 2, 2, 4)
}

# Sprite 22: Line (edge view)
Draw-At 6 1 {
    param($x, $y)
    $g.FillRectangle($goldBrush, $x + 3, $y + 2, 2, 4)
    $g.DrawRectangle([System.Drawing.Pens]::Black, $x + 3, $y + 2, 1, 3)
}

# Sprite 23: Oval other side (turning back)
Draw-At 7 1 {
    param($x, $y)
    $g.FillEllipse($goldBrush, $x + 3, $y + 2, 2, 4)
    $g.DrawEllipse([System.Drawing.Pens]::Black, $x + 3, $y + 2, 2, 4)
}

# ENEMY IDLE ANIMATION (sprites 30, 31)
# Sprite 30: Normal
Draw-At 14 1 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::Red, $x + 1, $y + 1, 6, 6)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 2, 1, 1)  # Left eye
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 4, $y + 2, 1, 1)  # Right eye
}

# Sprite 31: Blink/shifted
Draw-At 15 1 {
    param($x, $y)
    $g.FillRectangle([System.Drawing.Brushes]::Red, $x + 1, $y + 1, 6, 6)
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 2, $y + 3, 1, 1)  # Left eye lower
    $g.FillRectangle([System.Drawing.Brushes]::White, $x + 4, $y + 3, 1, 1)  # Right eye lower
}

# FIRE ANIMATION (sprites 40, 41, 42)
$orange = [System.Drawing.Color]::FromArgb(255, 140, 0)
$orangeBrush = New-Object System.Drawing.SolidBrush($orange)
$redOrange = [System.Drawing.Color]::FromArgb(255, 69, 0)
$redOrangeBrush = New-Object System.Drawing.SolidBrush($redOrange)

# Sprite 40: Small flame
Draw-At 8 2 {
    param($x, $y)
    $pts = @(
        [System.Drawing.Point]::new($x + 3, $y + 6),
        [System.Drawing.Point]::new($x + 2, $y + 4),
        [System.Drawing.Point]::new($x + 4, $y + 3),
        [System.Drawing.Point]::new($x + 5, $y + 4)
    )
    $g.FillPolygon($orangeBrush, $pts)
}

# Sprite 41: Medium flame
Draw-At 9 2 {
    param($x, $y)
    $pts = @(
        [System.Drawing.Point]::new($x + 3, $y + 7),
        [System.Drawing.Point]::new($x + 1, $y + 4),
        [System.Drawing.Point]::new($x + 4, $y + 2),
        [System.Drawing.Point]::new($x + 6, $y + 4)
    )
    $g.FillPolygon($redOrangeBrush, $pts)
}

# Sprite 42: Large flame
Draw-At 10 2 {
    param($x, $y)
    $pts = @(
        [System.Drawing.Point]::new($x + 3, $y + 7),
        [System.Drawing.Point]::new($x + 1, $y + 3),
        [System.Drawing.Point]::new($x + 4, $y + 1),
        [System.Drawing.Point]::new($x + 6, $y + 3)
    )
    $g.FillPolygon([System.Drawing.Brushes]::Yellow, $pts)
}

$g.Dispose()
$bmp.Save("$PSScriptRoot\cartridges\test_animations\spritesheet.png", [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()

Write-Host "Created improved 128x128 spritesheet with better animations!"
