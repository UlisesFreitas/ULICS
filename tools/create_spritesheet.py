from PIL import Image, ImageDraw

# Create 128x128 black image
img = Image.new('RGB', (128, 128), color='black')
draw = ImageDraw.Draw(img)

# Helper to draw a sprite at grid position
def draw_sprite(x_grid, y_grid, pixels):
    """Draw an 8x8 sprite at grid position (x_grid, y_grid)"""
    base_x = x_grid * 8
    base_y = y_grid * 8
    
    for py in range(8):
        for px in range(8):
            if py < len(pixels) and px < len(pixels[py]):
                color = pixels[py][px]
                if color:  # If not None/0
                    img.putpixel((base_x + px, base_y + py), color)

# Sprite 0: White square (player standing)
draw.rectangle([0, 2, 7, 7], fill=(255,255,255))
draw.rectangle([2, 2, 5, 5], fill=(0,0,0))  # Black center

# Sprite 1: White square offset (player walk 1)
draw.rectangle([8+1, 2, 8+6, 7], fill=(255,255,255))
draw.rectangle([8+2, 2, 8+4, 5], fill=(0,0,0))

# Sprite 2: White square offset (player walk 2)
draw.rectangle([16+2, 2, 16+7, 7], fill=(255,255,255))
draw.rectangle([16+3, 2, 16+6, 5], fill=(0,0,0))

# Sprite 10: Crouch (row 0, col 10 = x:80, y:0)
draw.rectangle([80, 4, 87, 7], fill=(255,255,255))

# Sprite 11: Jump (row 0, col 11 = x:88, y:0)
draw.ellipse([88+1, 1, 88+6, 6], fill=(255,255,255))

# Sprite 12: Land (row 0, col 12 = x:96, y:0)
draw.rectangle([96, 2, 103, 7], fill=(255,255,255))

# Sprite 20: Gold coin (row 1, col 4 = x:32, y:8)
draw.ellipse([32+1, 8+1, 32+6, 8+6], fill=(255,215,0))

# Sprite 21: Coin turning (row 1, col 5 = x:40, y:8)
draw.ellipse([40+2, 8+1, 40+5, 8+6], fill=(255,215,0))

# Sprite 22: Coin edge (row 1, col 6 = x:48, y:8)
draw.rectangle([48+3, 8+1, 48+4, 8+6], fill=(255,215,0))

# Sprite 23: Coin turning back (row 1, col 7 = x:56, y:8)
draw.ellipse([56+2, 8+1, 56+5, 8+6], fill=(255,215,0))

# Sprite 30: Enemy (row 1, col 14 = x:112, y:8)
draw.rectangle([112, 8, 119, 15], fill=(255,0,0))
draw.point((114, 10), fill=(255,255,255))  # Eye 1
draw.point((116, 10), fill=(255,255,255))  # Eye 2

# Sprite 31: Enemy blink (row 1, col 15 = x:120, y:8)
draw.rectangle([120, 8, 127, 15], fill=(255,0,0))
draw.point((122, 11), fill=(255,255,255))
draw.point((124, 11), fill=(255,255,255))

# Sprite 40: Fire small (row 2, col 8 = x:64, y:16)
points = [(64+3,16+6), (64+2,16+4), (64+4,16+4), (64+3,16+2)]
draw.polygon(points, fill=(255,140,0))

# Sprite 41: Fire medium (row 2, col 9 = x:72, y:16)
points = [(72+3,16+7), (72+1,16+4), (72+5,16+4), (72+3,16+1)]
draw.polygon(points, fill=(255,69,0))

# Sprite 42: Fire large (row 2, col 10 = x:80, y:16)
points = [(80+3,16+7), (80+1,16+4), (80+5,16+4), (80+3,16)]
draw.polygon(points, fill=(255,255,0))

# Save
img.save('spritesheet_128.png')
print("Created 128x128 spritesheet")
