/**
 * SpriteEditor_AnimationModal.cpp
 * Animation Editor Modal rendering
 * Shows animation details in an overlay
 */

#include "SpriteEditor.h"
#include "rendering/AestheticLayer.h"
#include "core/Engine.h"
#include "animation/AnimationManager.h"
#include "animation/Animation.h"
#include <iostream>

void SpriteEditor::RenderAnimationModal(AestheticLayer& renderer) {
    if (!animEditorOpen) return;
    if (!engineInstance) return;
    
    AnimationManager* animMgr = engineInstance->getAnimationManager();
    if (!animMgr) return;
    
    const auto& animations = animMgr->GetAllAnimations();
    if (selectedAnimIndex < 0 || selectedAnimIndex >= animations.size()) {
        animEditorOpen = false;  // Invalid index, close modal
        return;
    }
    
    const Animation& anim = animations[selectedAnimIndex];
    
    // Modal overlay (semi-transparent background)
    renderer.RectFillRGB(0, 0, 256, 256, 0, 0, 0);  // Full screen dark overlay
    
    // Modal window (centered)
    int modalX = 32;
    int modalY = 40;
    int modalW = 192;
    int modalH = 176;
    
    // Modal background
    renderer.RectFillRGB(modalX, modalY, modalW, modalH, 30, 30, 40);
    renderer.RectRGB(modalX, modalY, modalW, modalH, 255, 255, 255);  // White border
    
    // Title
    renderer.PrintRGB("ANIMATION INFO", modalX + 4, modalY + 4, 200, 200, 220);
    renderer.PrintRGB("[X] Close", modalX + modalW - 64, modalY + 4, 150, 150, 150);
    
    // Separator line
    renderer.LineRGB(modalX + 2, modalY + 14, modalX + modalW - 2, modalY + 14, 100, 100, 120);
    
    int yPos = modalY + 20;
    
    // Animation name
    renderer.PrintRGB("Name:", modalX + 4, yPos, 180, 180, 200);
    renderer.PrintRGB(anim.name.c_str(), modalX + 50, yPos, 200, 220, 200);
    yPos += 12;
    
    // Frame count
    char info[32];
    sprintf(info, "Frames: %d", static_cast<int>(anim.frames.size()));
    renderer.PrintRGB(info, modalX + 4, yPos, 180, 180, 200);
    yPos += 12;
    
    // Loop status
    sprintf(info, "Loop: %s", anim.loop ? "YES" : "NO");
    renderer.PrintRGB(info, modalX + 4, yPos, 180, 180, 200);
    yPos += 12;
    
    // Playing status
    sprintf(info, "Playing: %s", animMgr->IsPlaying(anim.name) ? "YES" : "NO");
    renderer.PrintRGB(info, modalX + 4, yPos, 180, 180, 200);
    yPos += 12;
    
    yPos += 6;  // Extra spacing
    
    // Show frame list
    renderer.PrintRGB("Frame IDs:", modalX + 4, yPos, 180, 180, 200);
    yPos += 10;
    
    // Display frames (max 8 per line)
    char frameList[64] = "";
    for (size_t i = 0; i < anim.frames.size() && i < 16; i++) {
        char temp[8];
        sprintf(temp, "%d ", anim.frames[i].spriteId);
        strcat(frameList, temp);
        
        if ((i + 1) % 8 == 0 || i == anim.frames.size() - 1) {
            renderer.PrintRGB(frameList, modalX + 8, yPos, 150, 200, 150);
            yPos += 10;
            frameList[0] = '\0';  // Clear
        }
    }
    
    yPos += 6;
    
    // Actions
    renderer.PrintRGB("Actions:", modalX + 4, yPos, 180, 180, 200);
    yPos += 12;
    
    // Play/Pause button
    if (animMgr->IsPlaying(anim.name)) {
        renderer.PrintRGB("[P] Pause", modalX + 8, yPos, 200, 180, 100);
    } else {
        renderer.PrintRGB("[P] Play", modalX + 8, yPos, 100, 200, 100);
    }
    yPos += 12;
    
    renderer.PrintRGB("[R] Reset", modalX + 8, yPos, 100, 180, 200);
    yPos += 12;
    
    renderer.PrintRGB("[S] Stop", modalX + 8, yPos, 200, 100, 100);
    
    // === SPRITE PREVIEW (right side) ===
    int previewX = modalX + modalW - 40;  // Right side
    int previewY = modalY + 50;
    int previewSize = 16;  // Display size: 16x16 (2x scale attempt)
    
    // Preview label
    renderer.PrintRGB("Preview:", previewX - 16, previewY - 12, 180, 180, 200);
    
    // Draw background box
    renderer.RectFillRGB(previewX - 2, previewY - 2, previewSize + 4, previewSize + 4, 10, 10, 20);
    renderer.RectRGB(previewX - 2, previewY - 2, previewSize + 4, previewSize + 4, 100, 100, 120);
    
    // Get current sprite for this animation (updates every frame!)
    int currentSpriteId = animMgr->GetCurrentSpriteId(anim.name);
    if (currentSpriteId >= 0 && currentSpriteId < 256) {
        // Simple approach: just draw the sprite at 2x2 grid for now
        // This should at least show the correct colors
        for (int ty = 0; ty < 2; ty++) {
            for (int tx = 0; tx < 2; tx++) {
                renderer.DrawSprite(currentSpriteId, 
                                   previewX + (tx * 8), 
                                   previewY + (ty * 8), 
                                   1, 1, false, false);
            }
        }
        
        // Current frame indicator
        char frameInfo[16];
        sprintf(frameInfo, "F:%d", currentSpriteId);
        renderer.PrintRGB(frameInfo, previewX + 2, previewY + previewSize + 6, 150, 200, 150);
    } else {
        renderer.PrintRGB("--", previewX + 8, previewY + 8, 100, 100, 100);
    }
    
    // Footer
    renderer.PrintRGB("(Full editor: future)", modalX + 4, modalY + modalH - 14, 100, 100, 100);
}
