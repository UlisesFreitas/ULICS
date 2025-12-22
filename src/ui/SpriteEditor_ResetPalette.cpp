void SpriteEditor::ResetPaletteToDefault() {
    Log("[Palette] Resetting to default palette...");
    
    if (!aestheticLayer) {
        Log("[Palette] ERROR: aestheticLayer is null");
        return;
    }
    
    // Reset palette to PICO-8/TIC-80 default
    aestheticLayer->ResetToDefaultPalette();
    
    // Save the default palette to the cartridge's palette.pal file
    SaveCartridgePalette();
    
    Log("[Palette] Reset to default and saved successfully");
}
