#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory> // Required for std::unique_ptr
#include "constants.h"
#include "utils.h"
#include "player.h"
#include "map.h"

class UI {
public:
    sf::Font font;
    sf::Text positionText;
    sf::Text chunkText;
    sf::Text instructionText;
    sf::Text fpsText;
    sf::Clock fpsTimer;
    int frameCount = 0;
    float fpsUpdateInterval = 1.0f;

    // Map system
    std::unordered_map<ChunkCoord, bool, ChunkCoordHash> exploredChunks;
    bool mapOpen = false;
    sf::RectangleShape minimapBackground;
    sf::RectangleShape mapBackground;
    sf::View mapView;

    // Inventory system
    bool inventoryOpen = false;
    bool craftingOpen = false;
    sf::RectangleShape inventoryBackground;
    sf::RectangleShape craftingBackground;
    sf::RectangleShape hotbarBackground;
    sf::RectangleShape slotBackground;
    sf::RectangleShape toolSlotBackground;
    sf::Text itemCountText;

    // Item textures for inventory display
    sf::Texture itemGrassTexture;
    sf::Texture itemWaterTexture;
    sf::Texture itemStoneTexture;
    sf::Texture itemTreeTexture;
    sf::Texture itemWoodTexture;
    sf::Texture itemStone2Texture;  // New stone texture for inventory
    sf::Texture itemWoodPickaxeTexture;
    sf::Texture itemWoodAxeTexture;
    sf::Texture itemDirtTexture;
    bool useItemTextures = false;

    // Crafting UI specific
    sf::Texture craftButtonTexture;
    bool useCraftButtonTexture = false;
    std::unique_ptr<sf::Sprite> craftButtonSprite; // Changed to unique_ptr
    int selectedCraftingRecipeIndex = -1; // -1 means no recipe selected

    // Inventory interaction
    int draggedSlot = -1;
    int draggedToolSlot = -1;
    bool isDragging = false;
    bool isDraggingFromTool = false;
    sf::Vector2f dragOffset;
    sf::Vector2f currentMousePos;

    // Inventory settings
    static const int SLOT_SIZE = 50;
    static const int SLOT_PADDING = 2;
    static const int INVENTORY_COLS = 10;
    static const int INVENTORY_ROWS = 10;
    static const int TOOL_SLOT_SIZE = 60;

    // Minimap settings
    static const int MINIMAP_SIZE = 200;
    static const int MINIMAP_TILE_SIZE = 2;
    static const int MINIMAP_RANGE = 25;

    // Full map settings
    static const int MAP_TILE_SIZE = 8;

    UI();

    void markChunkExplored(ChunkCoord chunk);
    sf::Color getBiomeColor(BiomeType biome);
    sf::Color getItemColor(int itemId);
    void drawMinimap(sf::RenderWindow& window, const Player& player, const Map& gameMap);
    void drawFullMap(sf::RenderWindow& window, const Player& player, const Map& gameMap);
    void drawHotbar(sf::RenderWindow& window, const Player& player);
    void drawInventory(sf::RenderWindow& window, const Player& player);
    void drawCrafting(sf::RenderWindow& window, const Player& player);
    void drawToolSlots(sf::RenderWindow& window, const Player& player);
    void drawInventorySlot(sf::RenderWindow& window, const InventorySlot& slot, sf::Vector2f position, bool selected = false);
    void drawDraggedItem(sf::RenderWindow& window, const Player& player, sf::Vector2f mousePos);
    void drawHarvestProgressBar(sf::RenderWindow& window, const Player& player);

    // Inventory interaction methods
    int getSlotAtPosition(sf::Vector2f mousePos, sf::Vector2f inventoryPos);
    int getToolSlotAtPosition(sf::Vector2f mousePos, sf::Vector2f toolSlotsPos);
    int getCraftingRecipeAtPosition(sf::Vector2f mousePos, sf::Vector2f craftingPos);
    bool isCraftButtonAtPosition(sf::Vector2f mousePos, sf::Vector2f craftingPos);
    void handleInventoryClick(sf::Vector2f mousePos, Player& player, bool isPressed);

    void toggleMap();
    void closeMap();
    bool isMapOpen() const;

    void toggleInventory();
    void closeInventory();
    bool isInventoryOpen() const;

    void toggleCrafting();
    void closeCrafting();
    bool isCraftingOpen() const;

    void update(const Player& player, int loadedChunks);
    void draw(sf::RenderWindow& window, const Player& player, const Map& gameMap);
};

#endif
