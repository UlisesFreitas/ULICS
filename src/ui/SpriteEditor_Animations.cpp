/**
 * SpriteEditor_Animations.cpp
 * Animation Panel rendering and interaction for Sprite Editor
 * Part of PHASE 3: UI - Animation Panel & Editor
 */

#include "SpriteEditor.h"
#include "rendering/AestheticLayer.h"
#include "core/Engine.h"
#include "animation/AnimationManager.h"
#include "animation/Animation.h"
#include <iostream>  // For debug logging

void SpriteEditor::RenderAnimationPanel(AestheticLayer& renderer) {
    // Panel background
    renderer.RectFillRGB(ANIM_PANEL_X, ANIM_PANEL_Y, ANIM_PANEL_W, ANIM_PANEL_H, 20, 20, 30);
    renderer.RectRGB(ANIM_PANEL_X, ANIM_PANEL_Y, ANIM_PANEL_W, ANIM_PANEL_H, 255, 255, 255);  //White border
    
    // Title
    renderer.PrintRGB("ANIMS", ANIM_PANEL_X + 2, ANIM_PANEL_Y + 2, 200, 200, 220);
    
    // Get AnimationManager from Engine
    if (!engineInstance) {
        renderer.PrintRGB("[NO ENGINE]", ANIM_PANEL_X + 2, ANIM_PANEL_Y + 14, 200, 50, 50);
        return;
    }
    
    AnimationManager* animMgr = engineInstance->getAnimationManager();
    if (!animMgr) {
        renderer.PrintRGB("[NO ANIMMGR]", ANIM_PANEL_X + 2, ANIM_PANEL_Y + 14, 200, 50, 50);
        return;
    }
    
    // Get all animations
    const auto& animations = animMgr->GetAllAnimations();
    
    if (animations.empty()) {
        renderer.PrintRGB("(none)", ANIM_PANEL_X + 2, ANIM_PANEL_Y + 14, 150, 150, 150);
        return;
    }
    
    // Render animation list (max 3 visible due to panel height)
    int yOffset = 14;
    int maxVisible = 3;
    for (size_t i = 0; i < animations.size() && i < maxVisible; i++) {
        const Animation& anim = animations[i];
        
        // Playing indicator (▶ or - )
        const char* playIcon = animMgr->IsPlaying(anim.name) ? ">" : "-";
        renderer.PrintRGB(playIcon, ANIM_PANEL_X + 2, ANIM_PANEL_Y + yOffset, 100, 200, 100);
        
        // Animation name
        renderer.PrintRGB(anim.name.c_str(), ANIM_PANEL_X + 10, ANIM_PANEL_Y + yOffset, 180, 220, 180);
        
        // Frame count (condensed to save space)
        char info[8];
        sprintf(info, "%d", static_cast<int>(anim.frames.size()));
       renderer.PrintRGB(info, ANIM_PANEL_X + 38, ANIM_PANEL_Y + yOffset, 120, 120, 150);
        
        // Loop indicator
        if (anim.loop) {
            renderer.PrintRGB("L", ANIM_PANEL_X + ANIM_PANEL_W - 10, ANIM_PANEL_Y + yOffset, 100, 200, 100);
        }
        
        yOffset += 12;
    }
    
    // Show count if more than maxVisible
    if (animations.size() > maxVisible) {
        char more[16];
        sprintf(more, "+%d more", static_cast<int>(animations.size() - maxVisible));
        renderer.PrintRGB(more, ANIM_PANEL_X + 2, ANIM_PANEL_Y + yOffset, 100, 100, 100);
    }
}

void SpriteEditor::HandleAnimationClick(int mouseX, int mouseY) {
    // Get AnimationManager
    if (!engineInstance) return;
    AnimationManager* animMgr = engineInstance->getAnimationManager();
    if (!animMgr) return;
    
    const auto& animations = animMgr->GetAllAnimations();
    if (animations.empty()) return;
    
    // Calculate relative position in panel
    int relY = mouseY - ANIM_PANEL_Y;
    
    // Skip title area (first 14px)
    if (relY < 14) return;
    
    // Check which animation was clicked (cada item = 12px height, empieza en y=14)
    int itemY = relY - 14;
    int clickedIndex = itemY / ANIM_ITEM_H;
    
    if (clickedIndex >= 0 && clickedIndex < animations.size() && clickedIndex < 3) {
        const Animation& anim = animations[clickedIndex];
        
        // Debug: Log the click
        char msg[64];
        sprintf(msg, "[AnimPanel] Clicked: %s", anim.name.c_str());
        std::cout << msg << std::endl;
        
        // Open animation details modal (simplified version for now)
        animEditorOpen = true;
        selectedAnimIndex = clickedIndex;
        editingAnimName = anim.name;
        
        std::cout << "  -> Opened animation details" << std::endl;
    }
}
