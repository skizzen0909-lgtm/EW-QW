// Systems/CraftingSystem.cpp
#include "CraftingSystem.h"
#include "Entities/Entity.h"
#include "Items/Item.h"
#include <iostream>
#include <algorithm>
#include <sstream>

CraftingSystem::CraftingSystem()
    : currentCraftTimer(0.0f)
    , craftingInProgress(false)
    , currentCrafter(nullptr) {
    initializeDefaultRecipes();
}

void CraftingSystem::registerRecipe(const CraftingRecipe& recipe) {
    recipes[recipe.id] = recipe;
}

void CraftingSystem::unregisterRecipe(const std::string& recipeId) {
    recipes.erase(recipeId);
}

bool CraftingSystem::canCraft(const std::string& recipeId, Entity* crafter) const {
    if (!crafter) return false;

    auto it = recipes.find(recipeId);
    if (it == recipes.end()) return false;

    const CraftingRecipe& recipe = it->second;

    if (!recipe.isUnlocked) return false;
    if (craftingInProgress) return false;

    for (const auto& ingredient : recipe.ingredients) {
        int count = 0;
        for (const auto& item : crafter->getInventory()) {
            if (item && item->getId() == ingredient.itemId) {
                count++;
            }
        }
        if (count < ingredient.quantity) return false;
    }

    return true;
}

bool CraftingSystem::startCrafting(const std::string& recipeId, Entity* crafter) {
    if (!canCraft(recipeId, crafter)) {
        std::cerr << "Невозможно начать крафт: " << recipeId << std::endl;
        return false;
    }

    const CraftingRecipe& recipe = recipes[recipeId];

    for (const auto& ingredient : recipe.ingredients) {
        crafter->removeItemById(ingredient.itemId, ingredient.quantity);
    }

    craftingInProgress = true;
    currentRecipeId = recipeId;
    currentCraftTimer = recipe.craftTime;
    currentCrafter = crafter;

    std::cout << crafter->getName() << " начинает крафт: " << recipe.name << std::endl;
    return true;
}

void CraftingSystem::update(float deltaTime) {
    if (!craftingInProgress) return;

    currentCraftTimer -= deltaTime;

    if (currentCraftTimer <= 0.0f) {
        craftingInProgress = false;

        auto it = recipes.find(currentRecipeId);
        if (it != recipes.end() && currentCrafter) {
            const CraftingRecipe& recipe = it->second;
            auto resultItem = std::make_shared<Item>(
                recipe.resultItemId, recipe.resultItemName, recipe.description,
                ItemType::MISC, ItemRarity::COMMON, 1);
            for (int i = 0; i < recipe.resultQuantity; ++i) {
                currentCrafter->addItem(resultItem);
            }
            std::cout << "[CRAFT] Крафт завершён: " << recipe.name << " x" << recipe.resultQuantity << std::endl;
        }

        currentCrafter = nullptr;
        currentRecipeId.clear();
    }
}

bool CraftingSystem::isCrafting() const {
    return craftingInProgress;
}

const CraftingRecipe* CraftingSystem::findRecipe(const std::string& recipeId) const {
    auto it = recipes.find(recipeId);
    return (it != recipes.end()) ? &it->second : nullptr;
}

std::vector<const CraftingRecipe*> CraftingSystem::getAvailableRecipes(Entity* crafter) const {
    std::vector<const CraftingRecipe*> result;
    for (const auto& [id, recipe] : recipes) {
        if (recipe.isUnlocked) {
            result.push_back(&recipe);
        }
    }
    return result;
}

std::vector<const CraftingRecipe*> CraftingSystem::getRecipesByCategory(const std::string& category) const {
    std::vector<const CraftingRecipe*> result;
    for (const auto& [id, recipe] : recipes) {
        if (recipe.skillCategory == category) {
            result.push_back(&recipe);
        }
    }
    return result;
}

void CraftingSystem::initializeDefaultRecipes() {
    // Зелья
    CraftingRecipe healthPotion;
    healthPotion.id = "craft_health_potion";
    healthPotion.name = "Зелье здоровья";
    healthPotion.description = "Восстанавливает 50 HP";
    healthPotion.ingredients = {
        {"herb", "Трава", 2},
        {"water_flask", "Флакон воды", 1}
    };
    healthPotion.resultItemId = "health_potion";
    healthPotion.resultItemName = "Зелье здоровья";
    healthPotion.resultQuantity = 1;
    healthPotion.requiredSkillLevel = 1;
    healthPotion.skillCategory = "alchemy";
    healthPotion.craftTime = 2.0f;
    registerRecipe(healthPotion);

    CraftingRecipe manaPotion;
    manaPotion.id = "craft_mana_potion";
    manaPotion.name = "Зелье маны";
    manaPotion.description = "Восстанавливает 30 MP";
    manaPotion.ingredients = {
        {"mana_crystal", "Кристалл маны", 1},
        {"water_flask", "Флакон воды", 1}
    };
    manaPotion.resultItemId = "mana_potion";
    manaPotion.resultItemName = "Зелье маны";
    manaPotion.resultQuantity = 1;
    manaPotion.requiredSkillLevel = 2;
    manaPotion.skillCategory = "alchemy";
    manaPotion.craftTime = 2.0f;
    registerRecipe(manaPotion);

    // Оружие
    CraftingRecipe ironSword;
    ironSword.id = "craft_iron_sword";
    ironSword.name = "Железный меч";
    ironSword.description = "Прочный железный меч";
    ironSword.ingredients = {
        {"iron_ingot", "Железный слиток", 3},
        {"leather_scrap", "Кожаный обрезок", 1}
    };
    ironSword.resultItemId = "iron_sword";
    ironSword.resultItemName = "Железный меч";
    ironSword.resultQuantity = 1;
    ironSword.requiredSkillLevel = 3;
    ironSword.skillCategory = "crafting";
    ironSword.craftTime = 4.0f;
    registerRecipe(ironSword);

    CraftingRecipe steelSword;
    steelSword.id = "craft_steel_sword";
    steelSword.name = "Стальной меч";
    steelSword.description = "Острый стальной меч";
    steelSword.ingredients = {
        {"steel_ingot", "Стальной слиток", 3},
        {"leather_scrap", "Кожаный обрезок", 2},
        {"ruby", "Рубин", 1}
    };
    steelSword.resultItemId = "steel_sword";
    steelSword.resultItemName = "Стальной меч";
    steelSword.resultQuantity = 1;
    steelSword.requiredSkillLevel = 7;
    steelSword.skillCategory = "crafting";
    steelSword.craftTime = 6.0f;
    registerRecipe(steelSword);

    // Броня
    CraftingRecipe leatherArmor;
    leatherArmor.id = "craft_leather_armor";
    leatherArmor.name = "Кожаная броня";
    leatherArmor.description = "Лёгкая кожаная броня";
    leatherArmor.ingredients = {
        {"leather_scrap", "Кожаный обрезок", 5},
        {"thread", "Нить", 3}
    };
    leatherArmor.resultItemId = "leather_armor";
    leatherArmor.resultItemName = "Кожаная броня";
    leatherArmor.resultQuantity = 1;
    leatherArmor.requiredSkillLevel = 2;
    leatherArmor.skillCategory = "crafting";
    leatherArmor.craftTime = 5.0f;
    registerRecipe(leatherArmor);

    // Еда
    CraftingRecipe cookedMeat;
    cookedMeat.id = "craft_cooked_meat";
    cookedMeat.name = "Жареное мясо";
    cookedMeat.description = "Восстанавливает 20 HP";
    cookedMeat.ingredients = {
        {"raw_meat", "Сырое мясо", 1}
    };
    cookedMeat.resultItemId = "cooked_meat";
    cookedMeat.resultItemName = "Жареное мясо";
    cookedMeat.resultQuantity = 1;
    cookedMeat.requiredSkillLevel = 1;
    cookedMeat.skillCategory = "survival";
    cookedMeat.craftTime = 1.5f;
    registerRecipe(cookedMeat);
}
