#include "ui/SystemSprites.h"
#include "rendering/AestheticLayer.h"
#include <cstring>
#include <iostream>

SystemSprites::SystemSprites() {
    std::memset(sprites, 0, sizeof(sprites));
}

SystemSprites::~SystemSprites() = default;

void SystemSprites::Initialize() {
    // Generate embedded default icons
    std::cout << "[SystemSprites] Initializing embedded icons..." << std::endl;
    GenerateDefaultIcons();
    std::cout << "[SystemSprites] 16 system icons ready (embedded in memory)" << std::endl;
}

void SystemSprites::DrawSprite(AestheticLayer& renderer, int spriteId, int x, int y, int scale) {
    if (spriteId < 0 || spriteId >= 256) return;
    
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            uint8_t color = sprites[spriteId][py][px];
            if (color != 0) {  // 0 = transparent
                renderer.RectFill(x + px * scale, y + py * scale, scale, scale, color);
            }
        }
    }
}

void SystemSprites::GenerateDefaultIcons() {
    // Clear all sprites first
    std::memset(sprites, 0, sizeof(sprites));
    
    GeneratePencilIcon();
    GenerateFillIcon();
    GenerateLineIcon();
    GenerateRectIcon();
    GeneratePickerIcon();
    GenerateSaveIcon();
    GenerateLoadIcon();
    GeneratePlayStopIcons();
    GenerateArrowIcons();
    GenerateFolderFileIcons();
    GenerateGearIcon();
}

// ===== ICON GENERATORS =====

void SystemSprites::GeneratePencilIcon() {
    // Sprite 0: Pencil (white only)
    uint8_t pencil[8][8] = {
        {0,0,0,0,0,0,7,7},
        {0,0,0,0,0,7,7,0},
        {0,0,0,0,7,7,0,0},
        {0,0,0,7,7,0,0,0},
        {0,0,7,7,0,0,0,0},
        {0,7,7,0,0,0,0,0},
        {7,7,0,0,0,0,0,0},
        {7,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::PENCIL)], pencil, 64);
}

void SystemSprites::GenerateFillIcon() {
    // Sprite 1: Paint bucket (white only)
    uint8_t bucket[8][8] = {
        {0,0,7,7,7,0,0,0},
        {0,7,0,0,0,7,0,0},
        {0,7,0,0,0,7,0,0},
        {0,0,7,0,7,0,0,0},
        {0,0,0,7,0,0,7,7},
        {0,0,7,7,7,7,7,0},
        {0,7,7,7,7,7,0,0},
        {0,0,7,7,7,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::FILL)], bucket, 64);
}

void SystemSprites::GenerateLineIcon() {
    // Sprite 2: Diagonal line
    uint8_t line[8][8] = {
        {7,0,0,0,0,0,0,0},
        {0,7,0,0,0,0,0,0},
        {0,0,7,0,0,0,0,0},
        {0,0,0,7,0,0,0,0},
        {0,0,0,0,7,0,0,0},
        {0,0,0,0,0,7,0,0},
        {0,0,0,0,0,0,7,0},
        {0,0,0,0,0,0,0,7}
    };
    std::memcpy(sprites[static_cast<int>(Icon::LINE)], line, 64);
}

void SystemSprites::GenerateRectIcon() {
    // Sprite 3: Rectangle outline
    uint8_t rect[8][8] = {
        {0,7,7,7,7,7,7,0},
        {0,7,0,0,0,0,7,0},
        {0,7,0,0,0,0,7,0},
        {0,7,0,0,0,0,7,0},
        {0,7,0,0,0,0,7,0},
        {0,7,0,0,0,0,7,0},
        {0,7,7,7,7,7,7,0},
        {0,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::RECT)], rect, 64);
}

void SystemSprites::GeneratePickerIcon() {
    // Sprite 4: Eyedropper (white only)
    uint8_t picker[8][8] = {
        {0,0,0,7,7,0,0,0},
        {0,0,7,0,0,7,0,0},
        {0,7,0,0,0,7,0,0},
        {7,7,0,0,7,0,0,0},
        {7,0,7,7,0,0,0,0},
        {7,0,7,0,0,0,0,0},
        {0,7,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::PICKER)], picker, 64);
}

void SystemSprites::GenerateSaveIcon() {
    // Sprite 5: Floppy disk
    uint8_t save[8][8] = {
        {7,7,7,7,7,7,7,7},
        {7,12,12,12,12,12,0,7},
        {7,12,12,12,12,12,0,7},
        {7,7,7,7,7,7,7,7},
        {7,5,5,5,5,5,5,7},
        {7,5,7,7,7,7,5,7},
        {7,5,5,5,5,5,5,7},
        {7,7,7,7,7,7,7,7}
    };
    std::memcpy(sprites[static_cast<int>(Icon::SAVE)], save, 64);
}

void SystemSprites::GenerateLoadIcon() {
    // Sprite 6: Folder open
    uint8_t load[8][8] = {
        {0,7,7,7,7,0,0,0},
        {7,14,14,14,7,7,0,0},
        {7,14,14,14,14,14,7,0},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,7,7,7,7,7,7,7}
    };
    std::memcpy(sprites[static_cast<int>(Icon::LOAD)], load, 64);
}

void SystemSprites::GeneratePlayStopIcons() {
    // Sprite 7: Play triangle
    uint8_t play[8][8] = {
        {0,0,11,0,0,0,0,0},
        {0,0,11,11,0,0,0,0},
        {0,0,11,11,11,0,0,0},
        {0,0,11,11,11,11,0,0},
        {0,0,11,11,11,0,0,0},
        {0,0,11,11,0,0,0,0},
        {0,0,11,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::PLAY)], play, 64);
    
    // Sprite 8: Stop square
    uint8_t stop[8][8] = {
        {0,0,0,0,0,0,0,0},
        {0,8,8,8,8,8,8,0},
        {0,8,8,8,8,8,8,0},
        {0,8,8,8,8,8,8,0},
        {0,8,8,8,8,8,8,0},
        {0,8,8,8,8,8,8,0},
        {0,8,8,8,8,8,8,0},
        {0,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::STOP)], stop, 64);
}

void SystemSprites::GenerateArrowIcons() {
    // Sprite 9: Left arrow
    uint8_t left[8][8] = {
        {0,0,0,7,0,0,0,0},
        {0,0,7,7,0,0,0,0},
        {0,7,7,7,7,7,7,0},
        {7,7,7,7,7,7,7,0},
        {7,7,7,7,7,7,7,0},
        {0,7,7,7,7,7,7,0},
        {0,0,7,7,0,0,0,0},
        {0,0,0,7,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::ARROW_LEFT)], left, 64);
    
    // Sprite 10: Right arrow
    uint8_t right[8][8] = {
        {0,0,0,7,0,0,0,0},
        {0,0,0,7,7,0,0,0},
        {0,7,7,7,7,7,7,0},
        {0,7,7,7,7,7,7,7},
        {0,7,7,7,7,7,7,7},
        {0,7,7,7,7,7,7,0},
        {0,0,0,7,7,0,0,0},
        {0,0,0,7,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::ARROW_RIGHT)], right, 64);
    
    // Sprite 11: Up arrow
    uint8_t up[8][8] = {
        {0,0,0,7,0,0,0,0},
        {0,0,7,7,7,0,0,0},
        {0,7,7,7,7,7,0,0},
        {7,7,7,7,7,7,7,0},
        {0,0,7,7,7,0,0,0},
        {0,0,7,7,7,0,0,0},
        {0,0,7,7,7,0,0,0},
        {0,0,0,0,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::ARROW_UP)], up, 64);
    
    // Sprite 12: Down arrow
    uint8_t down[8][8] = {
        {0,0,0,0,0,0,0,0},
        {0,0,7,7,7,0,0,0},
        {0,0,7,7,7,0,0,0},
        {0,0,7,7,7,0,0,0},
        {7,7,7,7,7,7,7,0},
        {0,7,7,7,7,7,0,0},
        {0,0,7,7,7,0,0,0},
        {0,0,0,7,0,0,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::ARROW_DOWN)], down, 64);
}

void SystemSprites::GenerateFolderFileIcons() {
    // Sprite 13: Folder closed
    uint8_t folder[8][8] = {
        {0,7,7,7,0,0,0,0},
        {7,14,14,14,7,7,7,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,14,14,14,14,14,14,7},
        {7,7,7,7,7,7,7,7}
    };
    std::memcpy(sprites[static_cast<int>(Icon::FOLDER)], folder, 64);
    
    // Sprite 14: File/Document
    uint8_t file[8][8] = {
        {0,7,7,7,7,7,0,0},
        {0,7,12,12,12,7,7,0},
        {0,7,12,12,12,12,7,0},
        {0,7,12,12,12,12,7,0},
        {0,7,12,12,12,12,7,0},
        {0,7,12,12,12,12,7,0},
        {0,7,12,12,12,12,7,0},
        {0,7,7,7,7,7,7,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::FILE)], file, 64);
}

void SystemSprites::GenerateGearIcon() {
    // Sprite 15: Settings/Gear
    uint8_t gear[8][8] = {
        {0,0,7,7,7,7,0,0},
        {0,7,13,13,13,13,7,0},
        {7,13,13,5,5,13,13,7},
        {7,13,5,5,5,5,13,7},
        {7,13,5,5,5,5,13,7},
        {7,13,13,5,5,13,13,7},
        {0,7,13,13,13,13,7,0},
        {0,0,7,7,7,7,0,0}
    };
    std::memcpy(sprites[static_cast<int>(Icon::GEAR)], gear, 64);
}
