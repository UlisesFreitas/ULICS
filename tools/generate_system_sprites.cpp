// Test program to generate default system_sprites.png
#include "ui/SystemSprites.h"
#include <iostream>

int main() {
    std::cout << "Generating default system_sprites.png..." << std::endl;
    
    SystemSprites sprites;
    sprites.Initialize();
    
    std::cout << "Done! Check assets/system_sprites.png" << std::endl;
    std::cout << "You can now edit it in any image editor or the Sprite Editor!" << std::endl;
    
    return 0;
}
