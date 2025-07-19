#include "player.h"
#include "map.h"
#include <cmath>
#include <iostream>
#include <algorithm>

Player::Player() {
    // Initialize inventory - start completely empty
    inventory.resize(INVENTORY_SIZE);

    // Initialize tool slots
    toolSlots.resize(static_cast<int>(ToolSlotType::TOOL_SLOT_COUNT));

    // Initialize crafting recipes
    initializeCraftingRecipes();

    // Don't add any test items - start with empty inventory

    // Try to load player texture
    if (!texture.loadFromFile("textures/player.png")) {
        std::cout << "Could not load player texture, using simple rectangle..." << std::endl;
        useSimpleGraphics = true;

        // Create a simple colored rectangle as fallback
        fallbackRect.setSize({ TILE_SIZE * 0.8f, TILE_SIZE * 0.8f });
        fallbackRect.setFillColor(sf::Color::Red);
        fallbackRect.setOrigin({ TILE_SIZE * 0.4f, TILE_SIZE * 0.4f }); // Center the sprite
    }
    else {
        // Set up sprite with loaded texture
        sprite.setTexture(texture);
        sprite = sf::Sprite(texture);

        // Scale the sprite to fit tile size (assuming player texture is smaller)
        sf::Vector2u textureSize = texture.getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            float scaleX = (TILE_SIZE * 0.8f) / textureSize.x;
            float scaleY = (TILE_SIZE * 0.8f) / textureSize.y;
            sprite.setScale({ scaleX, scaleY });
        }
    }

    // Find a safe spawn position in plains
    findSafeSpawnPosition();
}

void Player::initializeCraftingRecipes() {
    craftingRecipes.clear();

    // Wooden Pickaxe recipe
    CraftingRecipe pickaxeRecipe;
    pickaxeRecipe.resultItemId = 5; // WOOD_PICKAXE
    pickaxeRecipe.resultQuantity = 1;
    pickaxeRecipe.requiredItemId = 4; // WOOD
    pickaxeRecipe.requiredQuantity = 20;
    pickaxeRecipe.name = "Wooden Pickaxe";
    craftingRecipes.push_back(pickaxeRecipe);

    // Wooden Axe recipe
    CraftingRecipe axeRecipe;
    axeRecipe.resultItemId = 6; // WOOD_AXE
    axeRecipe.resultQuantity = 1;
    axeRecipe.requiredItemId = 4; // WOOD
    axeRecipe.requiredQuantity = 20;
    axeRecipe.name = "Wooden Axe";
    craftingRecipes.push_back(axeRecipe);
}

bool Player::addItem(int itemId, int quantity) {
    int maxStackSize = 64; // Default stack size
    if (itemId == 4) { // Wood item
        maxStackSize = 200;
    }

    // First try to add to existing stacks
    for (auto& slot : inventory) {
        if (slot.itemId == itemId && slot.quantity < maxStackSize) {
            int canAdd = std::min(quantity, maxStackSize - slot.quantity);
            slot.quantity += canAdd;
            quantity -= canAdd;
            if (quantity <= 0) return true;
        }
    }

    // Then try to add to empty slots
    for (auto& slot : inventory) {
        if (slot.isEmpty()) {
            slot.itemId = itemId;
            slot.quantity = std::min(quantity, maxStackSize);
            quantity -= slot.quantity;
            if (quantity <= 0) return true;
        }
    }

    return quantity <= 0; // Return true if all items were added
}

bool Player::removeItem(int itemId, int quantity) {
    for (auto& slot : inventory) {
        if (slot.itemId == itemId) {
            int canRemove = std::min(quantity, slot.quantity);
            slot.quantity -= canRemove;
            quantity -= canRemove;

            if (slot.quantity <= 0) {
                slot.clear();
            }

            if (quantity <= 0) return true;
        }
    }

    return quantity <= 0;
}

int Player::getItemCount(int itemId) const {
    int total = 0;
    for (const auto& slot : inventory) {
        if (slot.itemId == itemId) {
            total += slot.quantity;
        }
    }
    return total;
}

bool Player::moveItem(int fromSlot, int toSlot) {
    if (fromSlot < 0 || fromSlot >= INVENTORY_SIZE || toSlot < 0 || toSlot >= INVENTORY_SIZE) {
        return false;
    }

    if (fromSlot == toSlot) {
        return false;
    }

    InventorySlot& from = inventory[fromSlot];
    InventorySlot& to = inventory[toSlot];

    if (from.isEmpty()) {
        return false;
    }

    // If destination is empty, just move the item
    if (to.isEmpty()) {
        to = from;
        from.clear();
        return true;
    }

    // If both slots have the same item type, try to stack them
    if (from.itemId == to.itemId) {
        int maxStackSize = 64;
        if (from.itemId == 4) { // Wood item
            maxStackSize = 200;
        }

        int canMove = std::min(from.quantity, maxStackSize - to.quantity);
        if (canMove > 0) {
            to.quantity += canMove;
            from.quantity -= canMove;

            if (from.quantity <= 0) {
                from.clear();
            }
            return true;
        }
    }

    // If items are different or can't stack, swap them
    InventorySlot temp = from;
    from = to;
    to = temp;

    return true;
}

bool Player::moveItemToTool(int fromSlot, int toolSlot) {
    if (fromSlot < 0 || fromSlot >= INVENTORY_SIZE || toolSlot < 0 || toolSlot >= static_cast<int>(ToolSlotType::TOOL_SLOT_COUNT)) {
        return false;
    }

    InventorySlot& from = inventory[fromSlot];
    InventorySlot& to = toolSlots[toolSlot];

    if (from.isEmpty()) {
        return false;
    }

    // Check if item is valid for this tool slot
    bool validTool = false;
    if (toolSlot == static_cast<int>(ToolSlotType::PICKAXE) && from.itemId == 5) { // Wooden pickaxe
        validTool = true;
    }
    else if (toolSlot == static_cast<int>(ToolSlotType::AXE) && from.itemId == 6) { // Wooden axe
        validTool = true;
    }

    if (!validTool) {
        return false;
    }

    // Tool slots only hold one item
    if (!to.isEmpty()) {
        return false; // Tool slot already occupied
    }

    // Move one item to tool slot
    to.itemId = from.itemId;
    to.quantity = 1;
    from.quantity -= 1;

    if (from.quantity <= 0) {
        from.clear();
    }

    return true;
}

bool Player::moveItemFromTool(int toolSlot, int toSlot) {
    if (toolSlot < 0 || toolSlot >= static_cast<int>(ToolSlotType::TOOL_SLOT_COUNT) || toSlot < 0 || toSlot >= INVENTORY_SIZE) {
        return false;
    }

    InventorySlot& from = toolSlots[toolSlot];
    InventorySlot& to = inventory[toSlot];

    if (from.isEmpty()) {
        return false;
    }

    // If destination is empty, just move the item
    if (to.isEmpty()) {
        to = from;
        from.clear();
        return true;
    }

    // If same item type, try to stack
    if (from.itemId == to.itemId) {
        int maxStackSize = 64;
        int canMove = std::min(from.quantity, maxStackSize - to.quantity);
        if (canMove > 0) {
            to.quantity += canMove;
            from.quantity -= canMove;

            if (from.quantity <= 0) {
                from.clear();
            }
            return true;
        }
    }

    return false; // Can't move if destination is occupied with different item
}

bool Player::canCraft(const CraftingRecipe& recipe) const {
    return getItemCount(recipe.requiredItemId) >= recipe.requiredQuantity;
}

bool Player::craft(const CraftingRecipe& recipe) {
    if (!canCraft(recipe)) {
        return false;
    }

    // Remove required items
    if (!removeItem(recipe.requiredItemId, recipe.requiredQuantity)) {
        return false;
    }

    // Add result item
    if (!addItem(recipe.resultItemId, recipe.resultQuantity)) {
        // If we can't add the result, give back the materials
        addItem(recipe.requiredItemId, recipe.requiredQuantity);
        return false;
    }

    std::cout << "Crafted " << recipe.name << "!" << std::endl;
    return true;
}

bool Player::hasToolEquipped(ToolSlotType toolType) const {
    int slotIndex = static_cast<int>(toolType);
    return !toolSlots[slotIndex].isEmpty();
}

int Player::getEquippedTool(ToolSlotType toolType) const {
    int slotIndex = static_cast<int>(toolType);
    if (toolSlots[slotIndex].isEmpty()) {
        return -1;
    }
    return toolSlots[slotIndex].itemId;
}

float Player::getHarvestSpeedMultiplier() const {
    if (harvestTargetType == TileType::TREE && hasToolEquipped(ToolSlotType::AXE)) {
        return 1.25f; // 25% faster with axe
    }
    return 1.0f;
}

bool Player::canHarvestTile(TileType tileType) const {
    if (tileType == TileType::TREE) {
        return true; // Can always harvest trees
    }
    if (tileType == TileType::STONE) {
        return hasToolEquipped(ToolSlotType::PICKAXE); // Need pickaxe for stone
    }
    return false;
}

void Player::findSafeSpawnPosition() {
    // Start from center and spiral outward to find plains
    int centerX = WORLD_WIDTH / 2;
    int centerY = WORLD_HEIGHT / 2;

    for (int radius = 0; radius < 100; radius++) {
        for (int angle = 0; angle < 360; angle += 10) {
            int x = centerX + static_cast<int>(radius * std::cos(angle * M_PI / 180));
            int y = centerY + static_cast<int>(radius * std::sin(angle * M_PI / 180));

            if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
                // Check if this position is in plains (grassland)
                Map tempMap; // Temporary map for biome checking
                BiomeType biome = tempMap.determineBiome(x, y);
                TileType tileType = tempMap.generateTileType(x, y);

                if (biome == BiomeType::GRASSLAND && tileType == TileType::GRASS) {
                    setPosition({ static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE) });
                    return;
                }
            }
        }
    }

    // Fallback to center if no plains found
    setPosition({ static_cast<float>(WORLD_WIDTH * TILE_SIZE / 2), static_cast<float>(WORLD_HEIGHT * TILE_SIZE / 2) });
}

float Player::getCurrentMaxSpeed() const {
    return sprinting ? maxSpeed * sprintMultiplier : maxSpeed;
}

void Player::setPosition(const sf::Vector2f& position) {
    if (useSimpleGraphics) {
        fallbackRect.setPosition(position);
    }
    else {
        sprite.setPosition(position);
    }
}

sf::Vector2f Player::getPosition() const {
    if (useSimpleGraphics) {
        return fallbackRect.getPosition();
    }
    else {
        return sprite.getPosition();
    }
}

void Player::update(float dt, const Map& gameMap) {
    float currentMaxSpeed = getCurrentMaxSpeed();

    // Calculate target velocity
    sf::Vector2f targetVelocity{ 0.0f, 0.0f };

    if (movingLeft) targetVelocity.x -= currentMaxSpeed;
    if (movingRight) targetVelocity.x += currentMaxSpeed;
    if (movingUp) targetVelocity.y -= currentMaxSpeed;
    if (movingDown) targetVelocity.y += currentMaxSpeed;

    // Normalize diagonal movement
    if (targetVelocity.x != 0 && targetVelocity.y != 0) {
        float length = std::sqrt(targetVelocity.x * targetVelocity.x + targetVelocity.y * targetVelocity.y);
        targetVelocity.x = (targetVelocity.x / length) * currentMaxSpeed;
        targetVelocity.y = (targetVelocity.y / length) * currentMaxSpeed;
    }

    // Smooth velocity interpolation with adjusted acceleration for sprinting
    sf::Vector2f velocityDiff = targetVelocity - velocity;
    float changeAmount = (targetVelocity.x == 0 && targetVelocity.y == 0) ? friction : acceleration;

    // Increase acceleration when sprinting for more responsive feel
    if (sprinting && (targetVelocity.x != 0 || targetVelocity.y != 0)) {
        changeAmount *= 1.5f;
    }

    velocity.x += (velocityDiff.x > 0 ? 1 : -1) * std::min(std::abs(velocityDiff.x), changeAmount * dt);
    velocity.y += (velocityDiff.y > 0 ? 1 : -1) * std::min(std::abs(velocityDiff.y), changeAmount * dt);

    // Movement with collision detection
    sf::Vector2f originalPos = getPosition();

    // Try horizontal movement
    sf::Vector2f newPos = originalPos + sf::Vector2f{ velocity.x * dt, 0 };
    setPosition(newPos);

    // Simple collision check (just check player center)
    int tileX = static_cast<int>(newPos.x / TILE_SIZE);
    int tileY = static_cast<int>(newPos.y / TILE_SIZE);

    if (gameMap.isTileSolid(tileX, tileY)) {
        setPosition({ originalPos.x, newPos.y });
        velocity.x = 0;
    }

    // Try vertical movement
    sf::Vector2f currentPos = getPosition();
    newPos = currentPos + sf::Vector2f{ 0, velocity.y * dt };
    setPosition(newPos);

    tileX = static_cast<int>(newPos.x / TILE_SIZE);
    tileY = static_cast<int>(newPos.y / TILE_SIZE);

    if (gameMap.isTileSolid(tileX, tileY)) {
        setPosition({ currentPos.x, originalPos.y });
        velocity.y = 0;
    }

    // Update harvesting
    updateHarvesting(dt, const_cast<Map&>(gameMap));

    // World bounds
    sf::Vector2f pos = getPosition();
    if (pos.x < 0) setPosition({ 0, pos.y });
    if (pos.y < 0) setPosition({ pos.x, 0 });
    if (pos.x > WORLD_WIDTH * TILE_SIZE) setPosition({ static_cast<float>(WORLD_WIDTH * TILE_SIZE), pos.y });
    if (pos.y > WORLD_HEIGHT * TILE_SIZE) setPosition({ pos.x, static_cast<float>(WORLD_HEIGHT * TILE_SIZE) });
}

void Player::setMovement(bool left, bool right, bool up, bool down) {
    movingLeft = left;
    movingRight = right;
    movingUp = up;
    movingDown = down;
}

void Player::setSprinting(bool isSprinting) {
    sprinting = isSprinting;
}

sf::Vector2f Player::getWorldPosition() const {
    sf::Vector2f pos = getPosition();
    return sf::Vector2f{ pos.x / TILE_SIZE, pos.y / TILE_SIZE };
}

void Player::startHarvesting(int worldX, int worldY, sf::Vector2f targetPos, TileType tileType) {
    if (isWithinHarvestRange(worldX, worldY) && canHarvestTile(tileType)) {
        isHarvesting = true;
        harvestProgress = 0.0f;
        harvestTarget = targetPos;
        harvestTargetX = worldX;
        harvestTargetY = worldY;
        harvestTargetType = tileType;

        // Adjust harvest duration based on tools
        harvestDuration = 5.0f / getHarvestSpeedMultiplier();
    }
}

void Player::stopHarvesting() {
    isHarvesting = false;
    harvestProgress = 0.0f;
    harvestTargetX = -1;
    harvestTargetY = -1;
    harvestTargetType = TileType::GRASS;
}

bool Player::isWithinHarvestRange(int worldX, int worldY) const {
    sf::Vector2f playerWorldPos = getWorldPosition();
    int playerTileX = static_cast<int>(playerWorldPos.x);
    int playerTileY = static_cast<int>(playerWorldPos.y);

    int distanceX = std::abs(worldX - playerTileX);
    int distanceY = std::abs(worldY - playerTileY);

    return (distanceX <= 3 && distanceY <= 3);
}

void Player::updateHarvesting(float dt, Map& gameMap) {
    if (!isHarvesting) return;

    // Check if still in range
    if (!isWithinHarvestRange(harvestTargetX, harvestTargetY)) {
        stopHarvesting();
        return;
    }

    // Update progress
    harvestProgress += dt;

    // Check if harvesting is complete
    if (harvestProgress >= harvestDuration) {
        // Complete harvesting
        if (harvestTargetType == TileType::TREE) {
            if (gameMap.destroyTree(harvestTargetX, harvestTargetY)) {
                bool success = addItem(4, 10); // Add 10 wood (item ID 4)
                std::cout << "Tree harvested! Wood added to inventory: " << (success ? "Success" : "Failed - Inventory Full") << std::endl;
                std::cout << "Current wood count: " << getItemCount(4) << std::endl;
            }
        }
        else if (harvestTargetType == TileType::STONE) {
            if (gameMap.destroyStone(harvestTargetX, harvestTargetY)) {
                bool success = addItem(2, 5); // Add 5 stone (item ID 2)
                std::cout << "Stone harvested! Stone added to inventory: " << (success ? "Success" : "Failed - Inventory Full") << std::endl;
                std::cout << "Current stone count: " << getItemCount(2) << std::endl;
            }
        }
        stopHarvesting();
    }
}
