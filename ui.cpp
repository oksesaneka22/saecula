#include "ui.h"
#include <iostream>

UI::UI() : positionText(font), chunkText(font), instructionText(font), fpsText(font), itemCountText(font) {
    // Use default font if loading fails
    if (!font.openFromFile("fonts/arial.ttf")) {
        std::cout << "Using default font" << std::endl;
    }

    // Try to load item textures for inventory display
    if (itemGrassTexture.loadFromFile("textures/grass.png") &&
        itemWaterTexture.loadFromFile("textures/water.png") &&
        itemStoneTexture.loadFromFile("textures/stone.png") &&
        itemTreeTexture.loadFromFile("textures/tree.png") &&
        itemWoodTexture.loadFromFile("textures/wood.png") &&
        itemStone2Texture.loadFromFile("textures/stone2.png") &&
        itemWoodPickaxeTexture.loadFromFile("textures/wood_pickaxe.png") &&
        itemWoodAxeTexture.loadFromFile("textures/wood_axe.png") &&
        itemDirtTexture.loadFromFile("textures/dirt.png")) {
        useItemTextures = true;
        std::cout << "Item textures loaded successfully" << std::endl;
    }
    else {
        std::cout << "Using colored rectangles for inventory items" << std::endl;
        useItemTextures = false;
    }

    // Try to load craft button texture
    if (craftButtonTexture.loadFromFile("textures/craft_button.png")) {
        useCraftButtonTexture = true;
        craftButtonSprite = std::make_unique<sf::Sprite>(craftButtonTexture); // Construct sprite here
        // Scale the button if needed, assuming a reasonable default size
        sf::Vector2u textureSize = craftButtonTexture.getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            float scaleX = 150.0f / textureSize.x; // Target width 150
            float scaleY = 50.0f / textureSize.y;  // Target height 50
            craftButtonSprite->setScale({ scaleX, scaleY }); // Use -> for pointer
        }
    }
    else {
        std::cout << "Could not load craft_button.png, using simple rectangle for craft button." << std::endl;
        useCraftButtonTexture = false;
    }


    positionText.setFont(font);
    positionText.setString("Position: (0, 0)");
    positionText.setCharacterSize(16);
    positionText.setFillColor(sf::Color::White);

    chunkText.setFont(font);
    chunkText.setString("Loaded Chunks: 0");
    chunkText.setCharacterSize(16);
    chunkText.setFillColor(sf::Color::White);

    instructionText.setFont(font);
    instructionText.setString("WASD to move | SHIFT to sprint | M for map | E for inventory | C for crafting | Right-click to harvest");
    instructionText.setCharacterSize(14);
    instructionText.setFillColor(sf::Color::Yellow);

    fpsText.setFont(font);
    fpsText.setString("FPS: 0");
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);

    itemCountText.setFont(font);
    itemCountText.setCharacterSize(18);
    itemCountText.setFillColor(sf::Color::White);
    itemCountText.setStyle(sf::Text::Bold);

    // Setup minimap background
    minimapBackground.setSize({ static_cast<float>(MINIMAP_SIZE + 10), static_cast<float>(MINIMAP_SIZE + 10) });
    minimapBackground.setFillColor({ 0, 0, 0, 150 });
    minimapBackground.setOutlineColor(sf::Color::White);
    minimapBackground.setOutlineThickness(2);

    // Setup map background
    mapBackground.setSize({ 1200.0f, 800.0f });
    mapBackground.setFillColor({ 0, 0, 0, 200 });
    mapBackground.setOutlineColor(sf::Color::White);
    mapBackground.setOutlineThickness(3);

    // Setup inventory backgrounds
    inventoryBackground.setSize({
        static_cast<float>(INVENTORY_COLS * (SLOT_SIZE + SLOT_PADDING) + SLOT_PADDING + 200), // Extra space for tool slots
        static_cast<float>(INVENTORY_ROWS * (SLOT_SIZE + SLOT_PADDING) + SLOT_PADDING + 40)
        });
    inventoryBackground.setFillColor({ 0, 0, 0, 200 });
    inventoryBackground.setOutlineColor(sf::Color::White);
    inventoryBackground.setOutlineThickness(2);

    // Setup crafting background
    craftingBackground.setSize({ 400.0f, 500.0f });
    craftingBackground.setFillColor({ 0, 0, 0, 200 });
    craftingBackground.setOutlineColor(sf::Color::White);
    craftingBackground.setOutlineThickness(2);

    hotbarBackground.setSize({
        static_cast<float>(Player::HOTBAR_SIZE * (SLOT_SIZE + SLOT_PADDING) + SLOT_PADDING),
        static_cast<float>(SLOT_SIZE + SLOT_PADDING * 2)
        });
    hotbarBackground.setFillColor({ 0, 0, 0, 150 });
    hotbarBackground.setOutlineColor(sf::Color::White);
    hotbarBackground.setOutlineThickness(2);

    slotBackground.setSize({ static_cast<float>(SLOT_SIZE), static_cast<float>(SLOT_SIZE) });
    slotBackground.setFillColor({ 64, 64, 64 });
    slotBackground.setOutlineColor(sf::Color::White);
    slotBackground.setOutlineThickness(1);

    toolSlotBackground.setSize({ static_cast<float>(TOOL_SLOT_SIZE), static_cast<float>(TOOL_SLOT_SIZE) });
    toolSlotBackground.setFillColor({ 32, 32, 64 });
    toolSlotBackground.setOutlineColor(sf::Color::Cyan);
    toolSlotBackground.setOutlineThickness(2);
}

sf::Color UI::getItemColor(int itemId) {
    switch (itemId) {
    case 0: return sf::Color{ 34, 139, 34 };   // Grass - green
    case 1: return sf::Color{ 30, 144, 255 };  // Water - blue
    case 2: return sf::Color{ 128, 128, 128 }; // Stone - gray
    case 3: return sf::Color{ 0, 100, 0 };     // Tree - dark green
    case 4: return sf::Color{ 139, 69, 19 };   // Wood - brown
    case 5: return sf::Color{ 160, 82, 45 };   // Wood Pickaxe - saddle brown
    case 6: return sf::Color{ 205, 133, 63 };  // Wood Axe - peru
    default: return sf::Color::White;
    }
}

void UI::drawInventorySlot(sf::RenderWindow& window, const InventorySlot& slot, sf::Vector2f position, bool selected) {
    // Draw slot background
    slotBackground.setPosition(position);
    if (selected) {
        slotBackground.setOutlineColor(sf::Color::Yellow);
        slotBackground.setOutlineThickness(3);
    }
    else {
        slotBackground.setOutlineColor(sf::Color::White);
        slotBackground.setOutlineThickness(1);
    }
    window.draw(slotBackground);

    // Draw item if slot is not empty
    if (!slot.isEmpty()) {
        if (useItemTextures) {
            // Use textures for items
            sf::Texture* texture = nullptr;

            switch (slot.itemId) {
            case 0: texture = &itemGrassTexture; break;
            case 1: texture = &itemWaterTexture; break;
            case 2: texture = &itemStone2Texture; break; // Use stone2 texture for inventory
            case 3: texture = &itemTreeTexture; break;
            case 4: texture = &itemWoodTexture; break;
            case 5: texture = &itemWoodPickaxeTexture; break;
            case 6: texture = &itemWoodAxeTexture; break;
            default: texture = &itemGrassTexture; break;
            }

            if (texture) {
                sf::Sprite itemSprite = sf::Sprite(*texture);
                itemSprite.setPosition({ position.x + 2, position.y + 2 });

                // Scale texture to fit slot
                sf::Vector2u textureSize = texture->getSize();
                if (textureSize.x > 0 && textureSize.y > 0) {
                    float scaleX = static_cast<float>(SLOT_SIZE - 4) / textureSize.x;
                    float scaleY = static_cast<float>(SLOT_SIZE - 4) / textureSize.y;
                    itemSprite.setScale({ scaleX, scaleY });
                }

                window.draw(itemSprite);
            }
        }
        else {
            // Use colored rectangles as fallback
            sf::RectangleShape itemRect;
            itemRect.setSize({ static_cast<float>(SLOT_SIZE - 4), static_cast<float>(SLOT_SIZE - 4) });
            itemRect.setPosition({ position.x + 2, position.y + 2 });
            itemRect.setFillColor(getItemColor(slot.itemId));
            window.draw(itemRect);
        }

        // Draw quantity number with white text and black outline
        sf::Text quantityText = sf::Text(font);
        quantityText.setString(std::to_string(slot.quantity));
        quantityText.setCharacterSize(16);
        quantityText.setFillColor(sf::Color::White);
        quantityText.setStyle(sf::Text::Bold);
        quantityText.setOutlineColor(sf::Color::Black);
        quantityText.setOutlineThickness(2);

        // Position text in bottom-right corner of slot
        sf::FloatRect textBounds = quantityText.getLocalBounds();
        quantityText.setPosition({
            position.x + SLOT_SIZE - textBounds.size.x - 4,
            position.y + SLOT_SIZE - textBounds.size.y - 4
            });

        window.draw(quantityText);
    }
}

void UI::drawToolSlots(sf::RenderWindow& window, const Player& player) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f toolSlotsPos{
        static_cast<float>((windowSize.x - inventoryBackground.getSize().x) / 2 + inventoryBackground.getSize().x - 180),
        static_cast<float>((windowSize.y - inventoryBackground.getSize().y) / 2 + 50)
    };

    // Draw tool slot labels and slots
    const std::vector<std::string> toolNames = { "Pickaxe", "Axe" };

    for (int i = 0; i < static_cast<int>(ToolSlotType::TOOL_SLOT_COUNT); i++) {
        sf::Vector2f slotPos{
            toolSlotsPos.x,
            toolSlotsPos.y + i * (TOOL_SLOT_SIZE + 20)
        };

        // Draw label
        sf::Text toolLabel = sf::Text(font);
        toolLabel.setString(toolNames[i] + ":");
        toolLabel.setCharacterSize(16);
        toolLabel.setFillColor(sf::Color::White);
        toolLabel.setPosition({ slotPos.x, slotPos.y - 20 });
        window.draw(toolLabel);

        // Draw tool slot background
        toolSlotBackground.setPosition(slotPos);
        window.draw(toolSlotBackground);

        // Draw tool if equipped
        const InventorySlot& toolSlot = player.getToolSlots()[i];
        if (!toolSlot.isEmpty()) {
            if (useItemTextures) {
                sf::Texture* texture = nullptr;

                switch (toolSlot.itemId) {
                case 5: texture = &itemWoodPickaxeTexture; break;
                case 6: texture = &itemWoodAxeTexture; break;
                default: break;
                }

                if (texture) {
                    sf::Sprite toolSprite = sf::Sprite(*texture);
                    toolSprite.setPosition({ slotPos.x + 2, slotPos.y + 2 });

                    sf::Vector2u textureSize = texture->getSize();
                    if (textureSize.x > 0 && textureSize.y > 0) {
                        float scaleX = static_cast<float>(TOOL_SLOT_SIZE - 4) / textureSize.x;
                        float scaleY = static_cast<float>(TOOL_SLOT_SIZE - 4) / textureSize.y;
                        toolSprite.setScale({ scaleX, scaleY });
                    }

                    window.draw(toolSprite);
                }
            }
            else {
                sf::RectangleShape toolRect;
                toolRect.setSize({ static_cast<float>(TOOL_SLOT_SIZE - 4), static_cast<float>(TOOL_SLOT_SIZE - 4) });
                toolRect.setPosition({ slotPos.x + 2, slotPos.y + 2 });
                toolRect.setFillColor(getItemColor(toolSlot.itemId));
                window.draw(toolRect);
            }
        }
    }
}

void UI::drawCrafting(sf::RenderWindow& window, const Player& player) {
    if (!craftingOpen) return;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f craftingPos{
        static_cast<float>((windowSize.x - craftingBackground.getSize().x) / 2),
        static_cast<float>((windowSize.y - craftingBackground.getSize().y) / 2)
    };

    craftingBackground.setPosition(craftingPos);
    window.draw(craftingBackground);

    // Draw title
    sf::Text craftingTitle = sf::Text(font);
    craftingTitle.setString("Crafting (C to close) - Select a recipe and click CRAFT");
    craftingTitle.setCharacterSize(18);
    craftingTitle.setFillColor(sf::Color::White);
    craftingTitle.setPosition({ craftingPos.x + 10, craftingPos.y + 10 });
    window.draw(craftingTitle);

    // Draw crafting recipes
    const auto& recipes = player.getCraftingRecipes();
    float yOffset = 50;

    for (size_t i = 0; i < recipes.size(); i++) {
        const auto& recipe = recipes[i];

        sf::Vector2f recipePos{ craftingPos.x + 20, craftingPos.y + yOffset };

        // Highlight selected recipe
        sf::RectangleShape recipeHighlight;
        recipeHighlight.setSize({ craftingBackground.getSize().x - 40, 80 }); // Adjust size to fit recipe info
        recipeHighlight.setPosition({ recipePos.x - 10, recipePos.y - 10 });
        recipeHighlight.setFillColor({ 0, 0, 0, 0 }); // Transparent by default
        recipeHighlight.setOutlineColor(sf::Color::White);
        recipeHighlight.setOutlineThickness(1);

        if (static_cast<int>(i) == selectedCraftingRecipeIndex) {
            recipeHighlight.setFillColor({ 50, 50, 150, 100 }); // Blueish highlight
            recipeHighlight.setOutlineColor(sf::Color::Yellow);
            recipeHighlight.setOutlineThickness(2);
        }
        window.draw(recipeHighlight);

        // Check if player can craft this recipe
        bool canCraft = player.canCraft(recipe);
        sf::Color textColor = canCraft ? sf::Color::Green : sf::Color::Red;

        // Draw recipe name
        sf::Text recipeName = sf::Text(font);
        recipeName.setString(recipe.name);
        recipeName.setCharacterSize(16);
        recipeName.setFillColor(textColor);
        recipeName.setPosition(recipePos);
        window.draw(recipeName);

        // Draw requirements
        sf::Text requirements = sf::Text(font);
        requirements.setString("Requires: " + std::to_string(recipe.requiredQuantity) + " Wood");
        requirements.setCharacterSize(14);
        requirements.setFillColor(sf::Color::White);
        requirements.setPosition({ recipePos.x, recipePos.y + 20 });
        window.draw(requirements);

        // Draw current materials
        int currentMaterials = player.getItemCount(recipe.requiredItemId);
        sf::Text currentText = sf::Text(font);
        currentText.setString("You have: " + std::to_string(currentMaterials));
        currentText.setCharacterSize(14);
        currentText.setFillColor(currentMaterials >= recipe.requiredQuantity ? sf::Color::Green : sf::Color::Red);
        currentText.setPosition({ recipePos.x, recipePos.y + 40 });
        window.draw(currentText);

        yOffset += 100; // Spacing for next recipe
    }

    // Draw the single CRAFT button at the bottom

    sf::Vector2f craftButtonPos{
        craftingPos.x + (craftingBackground.getSize().x - (useCraftButtonTexture && craftButtonSprite ? craftButtonSprite->getGlobalBounds().size.x : 150.0f)) / 2,
        craftingPos.y + craftingBackground.getSize().y - 70
    };

    if (useCraftButtonTexture && craftButtonSprite) {
        craftButtonSprite->setPosition(craftButtonPos);
        window.draw(*craftButtonSprite);
    }
    else {
        // Fallback rectangle for craft button
        sf::RectangleShape craftButtonRect;
        craftButtonRect.setSize({ 150, 50 });
        craftButtonRect.setPosition(craftButtonPos);
        craftButtonRect.setFillColor(sf::Color{ 0, 150, 0 }); // Green color
        craftButtonRect.setOutlineColor(sf::Color::White);
        craftButtonRect.setOutlineThickness(2);
        window.draw(craftButtonRect);

        sf::Text craftButtonText = sf::Text(font);
        craftButtonText.setString("CRAFT");
        craftButtonText.setCharacterSize(20);
        craftButtonText.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = craftButtonText.getLocalBounds();
        craftButtonText.setPosition({
            craftButtonPos.x + (150 - textBounds.size.x) / 2,
            craftButtonPos.y + (50 - textBounds.size.y) / 2 - 5 // Adjust for vertical centering
            });
        window.draw(craftButtonText);
    }
}

int UI::getToolSlotAtPosition(sf::Vector2f mousePos, sf::Vector2f toolSlotsPos) {
    for (int i = 0; i < static_cast<int>(ToolSlotType::TOOL_SLOT_COUNT); i++) {
        sf::Vector2f slotPosition(toolSlotsPos.x, toolSlotsPos.y + i * (TOOL_SLOT_SIZE + 20));
        sf::Vector2f slotSize(static_cast<float>(TOOL_SLOT_SIZE), static_cast<float>(TOOL_SLOT_SIZE));

        // Manual bounds check to avoid sf::FloatRect::contains and direct width/height access
        //if (mousePos.x >= slotPosition.x && mousePos.x < slotPosition.x + slotSize.x &&
        //    mousePos.y >= slotPosition.y && mousePos.y < slotPosition.y + slotSize.y) {
        //    return i;
        //}
        sf::FloatRect slotRect(slotPosition, slotSize);
        if (slotRect.contains(mousePos)) {
            return i;
        }
    }
    return -1;
}

int UI::getCraftingRecipeAtPosition(sf::Vector2f mousePos, sf::Vector2f craftingPos) {
    const auto& recipes = Player().getCraftingRecipes();
    float yOffset = 50;

    for (size_t i = 0; i < recipes.size(); i++) {
        // Define the clickable area for each recipe entry
        sf::Vector2f recipeAreaPos(craftingPos.x + 10, craftingPos.y + yOffset - 10);
        sf::Vector2f recipeAreaSize(craftingBackground.getSize().x - 20, 80); // Match highlight size

        // Manual bounds check
        //if (mousePos.x >= recipeAreaPos.x && mousePos.x < recipeAreaPos.x + recipeAreaSize.x &&
        //    mousePos.y >= recipeAreaPos.y && mousePos.y < recipeAreaPos.y + recipeAreaSize.y) {
        //    return static_cast<int>(i);
        //}
        sf::FloatRect recipeRect(recipeAreaPos, recipeAreaSize);
        if (recipeRect.contains(mousePos)) {
            return static_cast<int>(i);
        }
        yOffset += 100;
    }
    return -1;
}

bool UI::isCraftButtonAtPosition(sf::Vector2f mousePos, sf::Vector2f craftingPos) {
    // Calculate button position based on whether texture is used or fallback rectangle
    //float buttonWidth = useCraftButtonTexture && craftButtonSprite ? craftButtonSprite->getGlobalBounds().width : 150.0f;
    //float buttonHeight = useCraftButtonTexture && craftButtonSprite ? craftButtonSprite->getGlobalBounds().height : 50.0f;
    float buttonWidth = useCraftButtonTexture && craftButtonSprite ? craftButtonSprite->getGlobalBounds().size.x : 150.0f;
    float buttonHeight = useCraftButtonTexture && craftButtonSprite ? craftButtonSprite->getGlobalBounds().size.y : 50.0f;

    sf::Vector2f craftButtonPos{
        craftingPos.x + (craftingBackground.getSize().x - buttonWidth) / 2,
        craftingPos.y + craftingBackground.getSize().y - 70
    };

    // Manual bounds check
    //return (mousePos.x >= craftButtonPos.x && mousePos.x < craftButtonPos.x + buttonWidth &&
    //        mousePos.y >= craftButtonPos.y && mousePos.y < craftButtonPos.y + buttonHeight);
    sf::FloatRect craftButtonRect(craftButtonPos, sf::Vector2f(buttonWidth, buttonHeight));
    return craftButtonRect.contains(mousePos);
}


void UI::handleInventoryClick(sf::Vector2f mousePos, Player& player, bool isPressed) {
    sf::Vector2u windowSize = sf::Vector2u{ 2560, 1440 };

    // Handle crafting clicks
    if (craftingOpen) {
        sf::Vector2f craftingPos{
            static_cast<float>((windowSize.x - craftingBackground.getSize().x) / 2),
            static_cast<float>((windowSize.y - craftingBackground.getSize().y) / 2)
        };

        if (isPressed) {
            int clickedRecipeIndex = getCraftingRecipeAtPosition(mousePos, craftingPos);
            if (clickedRecipeIndex != -1) {
                selectedCraftingRecipeIndex = clickedRecipeIndex;
            }
            else if (isCraftButtonAtPosition(mousePos, craftingPos)) {
                if (selectedCraftingRecipeIndex != -1) {
                    const auto& recipes = player.getCraftingRecipes();
                    if (selectedCraftingRecipeIndex < static_cast<int>(recipes.size())) {
                        player.craft(recipes[selectedCraftingRecipeIndex]);
                        // Optionally, deselect after crafting or keep selected
                        // selectedCraftingRecipeIndex = -1; 
                    }
                }
            }
        }
        return;
    }

    if (!inventoryOpen) return;

    sf::Vector2f invPos{
        static_cast<float>((windowSize.x - inventoryBackground.getSize().x) / 2),
        static_cast<float>((windowSize.y - inventoryBackground.getSize().y) / 2)
    };

    // Check tool slots first
    sf::Vector2f toolSlotsPos{
        invPos.x + inventoryBackground.getSize().x - 180,
        invPos.y + 50
    };

    int clickedToolSlot = getToolSlotAtPosition(mousePos, toolSlotsPos);
    int clickedSlot = getSlotAtPosition(mousePos, invPos);

    if (isPressed) {
        // Mouse button pressed
        if (clickedToolSlot >= 0 && !player.getToolSlots()[clickedToolSlot].isEmpty()) {
            draggedToolSlot = clickedToolSlot;
            isDraggingFromTool = true;
            isDragging = true;
        }
        else if (clickedSlot >= 0 && !player.inventory[clickedSlot].isEmpty()) {
            draggedSlot = clickedSlot;
            isDraggingFromTool = false;
            isDragging = true;
        }
    }
    else {
        // Mouse button released
        if (isDragging) {
            if (isDraggingFromTool && clickedSlot >= 0) {
                // Moving from tool slot to inventory
                player.moveItemFromTool(draggedToolSlot, clickedSlot);
            }
            else if (!isDraggingFromTool && clickedToolSlot >= 0) {
                // Moving from inventory to tool slot
                player.moveItemToTool(draggedSlot, clickedToolSlot);
            }
            else if (!isDraggingFromTool && clickedSlot >= 0 && draggedSlot != clickedSlot) {
                // Moving between inventory slots
                player.moveItem(draggedSlot, clickedSlot);
            }
        }
        isDragging = false;
        isDraggingFromTool = false;
        draggedSlot = -1;
        draggedToolSlot = -1;
    }
}

void UI::drawDraggedItem(sf::RenderWindow& window, const Player& player, sf::Vector2f mousePos) {
    if (!isDragging) return;

    const InventorySlot* slot = nullptr;

    if (isDraggingFromTool && draggedToolSlot >= 0) {
        slot = &player.getToolSlots()[draggedToolSlot];
    }
    else if (!isDraggingFromTool && draggedSlot >= 0 && draggedSlot < Player::INVENTORY_SIZE) {
        slot = &player.inventory[draggedSlot];
    }

    if (!slot || slot->isEmpty()) return;

    sf::Vector2f itemPos = mousePos - sf::Vector2f{ 25, 25 }; // Center on mouse

    // Draw semi-transparent background
    sf::RectangleShape dragBackground;
    dragBackground.setSize({ static_cast<float>(SLOT_SIZE), static_cast<float>(SLOT_SIZE) });
    dragBackground.setPosition(itemPos);
    dragBackground.setFillColor({ 64, 64, 64, 128 });
    dragBackground.setOutlineColor({ 255, 255, 255, 128 });
    dragBackground.setOutlineThickness(1);
    window.draw(dragBackground);

    // Draw the item
    if (useItemTextures) {
        sf::Texture* texture = nullptr;

        switch (slot->itemId) {
        case 0: texture = &itemGrassTexture; break;
        case 1: texture = &itemWaterTexture; break;
        case 2: texture = &itemStone2Texture; break;
        case 3: texture = &itemTreeTexture; break;
        case 4: texture = &itemWoodTexture; break;
        case 5: texture = &itemWoodPickaxeTexture; break;
        case 6: texture = &itemWoodAxeTexture; break;
        default: texture = &itemGrassTexture; break;
        }

        if (texture) {
            sf::Sprite itemSprite = sf::Sprite(*texture);
            itemSprite.setPosition({ itemPos.x + 2, itemPos.y + 2 });

            sf::Vector2u textureSize = texture->getSize();
            if (textureSize.x > 0 && textureSize.y > 0) {
                float scaleX = static_cast<float>(SLOT_SIZE - 4) / textureSize.x;
                float scaleY = static_cast<float>(SLOT_SIZE - 4) / textureSize.y;
                itemSprite.setScale({ scaleX, scaleY });
            }

            itemSprite.setColor({ 255, 255, 255, 200 }); // Semi-transparent
            window.draw(itemSprite);
        }
    }
    else {
        sf::RectangleShape itemRect;
        itemRect.setSize({ static_cast<float>(SLOT_SIZE - 4), static_cast<float>(SLOT_SIZE - 4) });
        itemRect.setPosition({ itemPos.x + 2, itemPos.y + 2 });
        sf::Color itemColor = getItemColor(slot->itemId);
        itemColor.a = 200; // Semi-transparent
        itemRect.setFillColor(itemColor);
        window.draw(itemRect);
    }

    // Draw quantity
    sf::Text dragQuantityText = sf::Text(font);
    dragQuantityText.setString(std::to_string(slot->quantity));
    dragQuantityText.setCharacterSize(16);
    dragQuantityText.setFillColor(sf::Color::White);
    dragQuantityText.setStyle(sf::Text::Bold);
    dragQuantityText.setOutlineColor(sf::Color::Black);
    dragQuantityText.setOutlineThickness(2);

    sf::FloatRect textBounds = dragQuantityText.getLocalBounds();
    dragQuantityText.setPosition({
        itemPos.x + SLOT_SIZE - textBounds.size.x - 4,
        itemPos.y + SLOT_SIZE - textBounds.size.y - 4
        });
    window.draw(dragQuantityText);
}

int UI::getSlotAtPosition(sf::Vector2f mousePos, sf::Vector2f inventoryPos) {
    // Check if mouse is within inventory bounds
    sf::Vector2f relativePos = mousePos - inventoryPos;
    relativePos.y -= 40; // Account for title area

    if (relativePos.x < SLOT_PADDING || relativePos.y < SLOT_PADDING) {
        return -1;
    }

    int col = static_cast<int>((relativePos.x - SLOT_PADDING) / (SLOT_SIZE + SLOT_PADDING));
    int row = static_cast<int>((relativePos.y - SLOT_PADDING) / (SLOT_SIZE + SLOT_PADDING));

    if (col >= 0 && col < INVENTORY_COLS && row >= 0 && row < INVENTORY_ROWS) {
        int slotIndex = row * INVENTORY_COLS + col;
        if (slotIndex < Player::INVENTORY_SIZE) {
            return slotIndex;
        }
    }

    return -1;
}

void UI::drawHotbar(sf::RenderWindow& window, const Player& player) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f hotbarPos{
        static_cast<float>((windowSize.x - hotbarBackground.getSize().x) / 2),
        static_cast<float>(windowSize.y - hotbarBackground.getSize().y - 20)
    };

    hotbarBackground.setPosition(hotbarPos);
    window.draw(hotbarBackground);

    // Draw hotbar slots
    for (int i = 0; i < Player::HOTBAR_SIZE; i++) {
        sf::Vector2f slotPos{
            hotbarPos.x + SLOT_PADDING + i * (SLOT_SIZE + SLOT_PADDING),
            hotbarPos.y + SLOT_PADDING
        };

        bool selected = (i == player.selectedHotbarSlot);
        drawInventorySlot(window, player.inventory[i], slotPos, selected);
    }
}

void UI::drawInventory(sf::RenderWindow& window, const Player& player) {
    if (!inventoryOpen) return;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f invPos{
        static_cast<float>((windowSize.x - inventoryBackground.getSize().x) / 2),
        static_cast<float>((windowSize.y - inventoryBackground.getSize().y) / 2)
    };

    inventoryBackground.setPosition(invPos);
    window.draw(inventoryBackground);

    // Draw title
    sf::Text invTitle = sf::Text(font);
    invTitle.setString("Inventory (E to close) - Click and drag to move items");
    invTitle.setCharacterSize(18);
    invTitle.setFillColor(sf::Color::White);
    invTitle.setPosition({ invPos.x + 10, invPos.y + 10 });
    window.draw(invTitle);

    // Draw inventory slots
    for (int row = 0; row < INVENTORY_ROWS; row++) {
        for (int col = 0; col < INVENTORY_COLS; col++) {
            int slotIndex = row * INVENTORY_COLS + col;
            if (slotIndex >= Player::INVENTORY_SIZE) break;

            sf::Vector2f slotPos{
                invPos.x + SLOT_PADDING + col * (SLOT_SIZE + SLOT_PADDING),
                invPos.y + 40 + SLOT_PADDING + row * (SLOT_SIZE + SLOT_PADDING)
            };

            // Highlight hotbar slots
            bool isHotbarSlot = (slotIndex < Player::HOTBAR_SIZE);

            // Don't draw the item if it's being dragged, but still draw the slot background
            if (isDragging && !isDraggingFromTool && slotIndex == draggedSlot) {
                // Draw empty slot
                InventorySlot emptySlot;
                drawInventorySlot(window, emptySlot, slotPos, isHotbarSlot);
            }
            else {
                drawInventorySlot(window, player.inventory[slotIndex], slotPos, isHotbarSlot);
            }
        }
    }

    // Draw tool slots
    drawToolSlots(window, player);

    // Draw dragged item last so it appears on top
    if (isDragging) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        drawDraggedItem(window, player, { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
    }
}

void UI::drawHarvestProgressBar(sf::RenderWindow& window, const Player& player) {
    if (!player.getIsHarvesting()) return;

    sf::Vector2u windowSize = window.getSize();
    float barWidth = 300.0f;
    float barHeight = 20.0f;

    sf::Vector2f barPos{
        static_cast<float>((windowSize.x - barWidth) / 2),
        static_cast<float>(windowSize.y / 2 - 50)
    };

    // Background
    sf::RectangleShape barBackground;
    barBackground.setSize({ barWidth, barHeight });
    barBackground.setPosition(barPos);
    barBackground.setFillColor({ 64, 64, 64 });
    barBackground.setOutlineColor(sf::Color::White);
    barBackground.setOutlineThickness(2);
    window.draw(barBackground);

    // Progress fill
    float progress = player.getHarvestProgress();
    sf::RectangleShape barFill;
    barFill.setSize({ barWidth * progress, barHeight });
    barFill.setPosition(barPos);
    barFill.setFillColor({ 0, 255, 0 });
    window.draw(barFill);

    // Text
    sf::Text harvestText = sf::Text(font);
    std::string harvestType = (player.harvestTargetType == TileType::TREE) ? "Tree" : "Stone";
    harvestText.setString("Harvesting " + harvestType + "...");
    harvestText.setCharacterSize(16);
    harvestText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = harvestText.getLocalBounds();
    harvestText.setPosition({
        barPos.x + (barWidth - textBounds.size.x) / 2,
        barPos.y - 25
        });
    window.draw(harvestText);
}

void UI::markChunkExplored(ChunkCoord chunk) {
    exploredChunks[chunk] = true;
}

sf::Color UI::getBiomeColor(BiomeType biome) {
    switch (biome) {
    case BiomeType::GRASSLAND: return sf::Color{ 34, 139, 34 };
    case BiomeType::FOREST: return sf::Color{ 0, 100, 0 };
    case BiomeType::MOUNTAIN: return sf::Color{ 128, 128, 128 };
    case BiomeType::LAKE: return sf::Color{ 30, 144, 255 };
    case BiomeType::RIVER: return sf::Color{ 30, 144, 255 };
    default: return sf::Color::Black;
    }
}

void UI::drawMinimap(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    sf::Vector2f playerPos = player.getWorldPosition();

    // Position minimap in top-right corner
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f minimapPos{ static_cast<float>(windowSize.x - MINIMAP_SIZE - 20), 20.0f };

    minimapBackground.setPosition({ minimapPos.x - 5, minimapPos.y - 5 });
    window.draw(minimapBackground);

    // Draw explored chunks on minimap
    int playerChunkX = static_cast<int>(playerPos.x) / CHUNK_SIZE;
    int playerChunkY = static_cast<int>(playerPos.y) / CHUNK_SIZE;

    for (int dy = -MINIMAP_RANGE; dy <= MINIMAP_RANGE; dy++) {
        for (int dx = -MINIMAP_RANGE; dx <= MINIMAP_RANGE; dx++) {
            int chunkX = playerChunkX + dx;
            int chunkY = playerChunkY + dy;

            if (chunkX >= 0 && chunkX < CHUNKS_X && chunkY >= 0 && chunkY < CHUNKS_Y) {
                ChunkCoord chunk = { chunkX, chunkY };

                if (exploredChunks.find(chunk) != exploredChunks.end()) {
                    // Sample biome from center of chunk
                    int sampleX = chunkX * CHUNK_SIZE + CHUNK_SIZE / 2;
                    int sampleY = chunkY * CHUNK_SIZE + CHUNK_SIZE / 2;
                    BiomeType biome = gameMap.determineBiome(sampleX, sampleY);

                    sf::RectangleShape chunkRect;
                    chunkRect.setSize({ static_cast<float>(MINIMAP_TILE_SIZE), static_cast<float>(MINIMAP_TILE_SIZE) });
                    chunkRect.setFillColor(getBiomeColor(biome));
                    chunkRect.setPosition({
                        minimapPos.x + (dx + MINIMAP_RANGE) * MINIMAP_TILE_SIZE,
                        minimapPos.y + (dy + MINIMAP_RANGE) * MINIMAP_TILE_SIZE
                        });
                    window.draw(chunkRect);
                }
            }
        }
    }

    // Draw player position on minimap with different color when sprinting
    sf::CircleShape playerDot(3);
    playerDot.setFillColor(player.sprinting ? sf::Color::Yellow : sf::Color::Red);
    playerDot.setPosition({
        minimapPos.x + MINIMAP_RANGE * MINIMAP_TILE_SIZE - 3,
        minimapPos.y + MINIMAP_RANGE * MINIMAP_TILE_SIZE - 3
        });
    window.draw(playerDot);
}

void UI::drawFullMap(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    if (!mapOpen) return;

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f mapPos{
        static_cast<float>((windowSize.x - mapBackground.getSize().x) / 2),
        static_cast<float>((windowSize.y - mapBackground.getSize().y) / 2)
    };

    mapBackground.setPosition(mapPos);
    window.draw(mapBackground);

    // Draw title
    sf::Text mapTitle = sf::Text(font);
    mapTitle.setString("Explored Map (M to close)");
    mapTitle.setCharacterSize(20);
    mapTitle.setFillColor(sf::Color::White);
    mapTitle.setPosition({ mapPos.x + 10, mapPos.y + 10 });
    window.draw(mapTitle);

    // Draw explored chunks
    sf::Vector2f playerPos = player.getWorldPosition();
    int playerChunkX = static_cast<int>(playerPos.x) / CHUNK_SIZE;
    int playerChunkY = static_cast<int>(playerPos.y) / CHUNK_SIZE;

    // Calculate map bounds
    float mapStartX = mapPos.x + 10;
    float mapStartY = mapPos.y + 40;
    float mapWidth = mapBackground.getSize().x - 20;
    float mapHeight = mapBackground.getSize().y - 50;

    // Calculate visible chunk range
    int chunksPerRow = static_cast<int>(mapWidth / MAP_TILE_SIZE);
    int chunksPerCol = static_cast<int>(mapHeight / MAP_TILE_SIZE);

    int startChunkX = playerChunkX - chunksPerRow / 2;
    int startChunkY = playerChunkY - chunksPerCol / 2;

    for (int y = 0; y < chunksPerCol; y++) {
        for (int x = 0; x < chunksPerRow; x++) {
            int chunkX = startChunkX + x;
            int chunkY = startChunkY + y;

            if (chunkX >= 0 && chunkX < CHUNKS_X && chunkY >= 0 && chunkY < CHUNKS_Y) {
                ChunkCoord chunk = { chunkX, chunkY };

                if (exploredChunks.find(chunk) != exploredChunks.end()) {
                    // Sample biome from center of chunk
                    int sampleX = chunkX * CHUNK_SIZE + CHUNK_SIZE / 2;
                    int sampleY = chunkY * CHUNK_SIZE + CHUNK_SIZE / 2;
                    BiomeType biome = gameMap.determineBiome(sampleX, sampleY);

                    sf::RectangleShape chunkRect;
                    chunkRect.setSize({ static_cast<float>(MAP_TILE_SIZE), static_cast<float>(MAP_TILE_SIZE) });
                    chunkRect.setFillColor(getBiomeColor(biome));
                    chunkRect.setPosition({
                        mapStartX + x * MAP_TILE_SIZE,
                        mapStartY + y * MAP_TILE_SIZE
                        });
                    window.draw(chunkRect);
                }
            }
        }
    }

    // Draw player position on full map with different color when sprinting
    sf::CircleShape playerDot(4);
    playerDot.setFillColor(player.sprinting ? sf::Color::Yellow : sf::Color::Red);
    playerDot.setOutlineColor(sf::Color::White);
    playerDot.setOutlineThickness(1);
    playerDot.setPosition({
        mapStartX + (chunksPerRow / 2) * MAP_TILE_SIZE - 4,
        mapStartY + (chunksPerCol / 2) * MAP_TILE_SIZE - 4
        });
    window.draw(playerDot);
}

void UI::toggleMap() {
    mapOpen = !mapOpen;
}

void UI::closeMap() {
    mapOpen = false;
}

bool UI::isMapOpen() const {
    return mapOpen;
}

void UI::toggleInventory() {
    inventoryOpen = !inventoryOpen;
}

void UI::closeInventory() {
    inventoryOpen = false;
}

bool UI::isInventoryOpen() const {
    return inventoryOpen;
}

void UI::toggleCrafting() {
    craftingOpen = !craftingOpen;
    if (!craftingOpen) {
        selectedCraftingRecipeIndex = -1; // Deselect recipe when closing crafting menu
    }
}

void UI::closeCrafting() {
    craftingOpen = false;
    selectedCraftingRecipeIndex = -1; // Deselect recipe when closing crafting menu
}

bool UI::isCraftingOpen() const {
    return craftingOpen;
}

void UI::update(const Player& player, int loadedChunks) {
    sf::Vector2f worldPos = player.getWorldPosition();
    std::string sprintStatus = player.sprinting ? " (SPRINTING)" : "";
    positionText.setString("Position: (" + std::to_string(static_cast<int>(worldPos.x)) +
        ", " + std::to_string(static_cast<int>(worldPos.y)) + ")" + sprintStatus);

    chunkText.setString("Loaded Chunks: " + std::to_string(loadedChunks));

    frameCount++;
    if (fpsTimer.getElapsedTime().asSeconds() >= fpsUpdateInterval) {
        float fps = frameCount / fpsTimer.getElapsedTime().asSeconds();
        fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        frameCount = 0;
        fpsTimer.restart();
    }

    // Mark current chunk as explored
    ChunkCoord currentChunk = {
        static_cast<int>(worldPos.x) / CHUNK_SIZE,
        static_cast<int>(worldPos.y) / CHUNK_SIZE
    };
    markChunkExplored(currentChunk);
}

void UI::draw(sf::RenderWindow& window, const Player& player, const Map& gameMap) {
    sf::View originalView = window.getView();
    window.setView(window.getDefaultView());

    positionText.setPosition({ 10, 10 });
    chunkText.setPosition({ 10, 30 });
    instructionText.setPosition({ 10, static_cast<float>(window.getSize().y - 50) });

    sf::FloatRect fpsRect = fpsText.getLocalBounds();
    fpsText.setPosition({
        static_cast<float>(window.getSize().x) - fpsRect.size.x - 10,
        static_cast<float>(window.getSize().y) - 30
        });

    window.draw(positionText);
    window.draw(chunkText);
    window.draw(instructionText);
    window.draw(fpsText);

    // Draw minimap (always visible)
    drawMinimap(window, player, gameMap);

    // Draw full map (only when open)
    drawFullMap(window, player, gameMap);

    // Draw hotbar (always visible at bottom)
    drawHotbar(window, player);

    // Draw full inventory (only when open)
    drawInventory(window, player);

    // Draw crafting interface (only when open)
    drawCrafting(window, player);

    // Draw harvest progress bar
    drawHarvestProgressBar(window, player);

    window.setView(originalView);
}
