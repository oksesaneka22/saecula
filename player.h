#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "constants.h"

class Map; // Forward declaration

struct InventorySlot {
    int itemId = -1;  // -1 means empty slot
    int quantity = 0;

    bool isEmpty() const { return itemId == -1 || quantity <= 0; }
    void clear() { itemId = -1; quantity = 0; }
};

struct CraftingRecipe {
    int resultItemId;
    int resultQuantity;
    int requiredItemId;
    int requiredQuantity;
    std::string name;
};

class Player {
public:
    sf::Texture texture;
    sf::Sprite sprite = sf::Sprite(texture);
    sf::RectangleShape fallbackRect;  // Fallback rectangle for when texture fails
    sf::Vector2f velocity;
    float speed = 200.0f;
    float maxSpeed = 200.0f;
    float sprintMultiplier = 3.0f;  // 3x speed when sprinting
    float acceleration = 800.0f;
    float friction = 600.0f;
    bool movingLeft = false;
    bool movingRight = false;
    bool movingUp = false;
    bool movingDown = false;
    bool sprinting = false;  // Sprint state
    bool useSimpleGraphics = false;  // Fallback flag

    // Inventory system
    static const int INVENTORY_SIZE = 100;
    static const int HOTBAR_SIZE = 10;
    std::vector<InventorySlot> inventory;
    int selectedHotbarSlot = 0;

    // Tool slots
    std::vector<InventorySlot> toolSlots;

    // Crafting system
    std::vector<CraftingRecipe> craftingRecipes;

    // Harvesting system
    bool isHarvesting = false;
    sf::Vector2f harvestTarget;
    float harvestProgress = 0.0f;
    float harvestDuration = 5.0f; // 5 seconds base
    int harvestTargetX = -1;
    int harvestTargetY = -1;
    TileType harvestTargetType = TileType::GRASS;

    Player();

    void initializeCraftingRecipes();
    void findSafeSpawnPosition();
    void update(float dt, const Map& gameMap);
    void setMovement(bool left, bool right, bool up, bool down);
    void setSprinting(bool isSprinting);

    sf::Vector2f getPosition() const;
    sf::Vector2f getWorldPosition() const;
    void setPosition(const sf::Vector2f& position);

    // Inventory methods
    bool addItem(int itemId, int quantity = 1);
    bool removeItem(int itemId, int quantity = 1);
    int getItemCount(int itemId) const;
    bool moveItem(int fromSlot, int toSlot); // Move between inventory slots
    bool moveItemToTool(int fromSlot, int toolSlot); // Move from inventory to tool slot
    bool moveItemFromTool(int toolSlot, int toSlot); // Move from tool slot to inventory
    const std::vector<InventorySlot>& getInventory() const { return inventory; }
    const std::vector<InventorySlot>& getToolSlots() const { return toolSlots; }

    // Crafting methods
    bool canCraft(const CraftingRecipe& recipe) const;
    bool craft(const CraftingRecipe& recipe);
    const std::vector<CraftingRecipe>& getCraftingRecipes() const { return craftingRecipes; }

    // Tool methods
    bool hasToolEquipped(ToolSlotType toolType) const;
    int getEquippedTool(ToolSlotType toolType) const;
    float getHarvestSpeedMultiplier() const;

    // Harvesting methods
    void startHarvesting(int worldX, int worldY, sf::Vector2f targetPos, TileType tileType);
    void stopHarvesting();
    void updateHarvesting(float dt, Map& gameMap);
    bool isWithinHarvestRange(int worldX, int worldY) const;
    bool canHarvestTile(TileType tileType) const;
    float getHarvestProgress() const { return harvestProgress / harvestDuration; }
    bool getIsHarvesting() const { return isHarvesting; }
    sf::Vector2f getHarvestTarget() const { return harvestTarget; }

private:
    float getCurrentMaxSpeed() const;
};

#endif
