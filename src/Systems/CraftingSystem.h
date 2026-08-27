// Systems/CraftingSystem.h
#ifndef CRAFTINGSYSTEM_H
#define CRAFTINGSYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Entity;
class Item;

struct CraftingIngredient {
    std::string itemId;
    std::string itemName;
    int quantity;
};

struct CraftingRecipe {
    std::string id;
    std::string name;
    std::string description;
    std::vector<CraftingIngredient> ingredients;
    std::string resultItemId;
    std::string resultItemName;
    int resultQuantity;
    int requiredSkillLevel;
    std::string skillCategory;
    float craftTime;
    bool isUnlocked;

    CraftingRecipe()
        : resultQuantity(1), requiredSkillLevel(1), craftTime(1.0f), isUnlocked(true) {}
};

class CraftingSystem {
private:
    std::unordered_map<std::string, CraftingRecipe> recipes;
    float currentCraftTimer;
    bool craftingInProgress;
    std::string currentRecipeId;
    Entity* currentCrafter;

public:
    CraftingSystem();
    ~CraftingSystem() = default;

    void registerRecipe(const CraftingRecipe& recipe);
    void unregisterRecipe(const std::string& recipeId);

    bool canCraft(const std::string& recipeId, Entity* crafter) const;
    bool startCrafting(const std::string& recipeId, Entity* crafter);
    void update(float deltaTime);
    bool isCrafting() const;

    const CraftingRecipe* findRecipe(const std::string& recipeId) const;
    std::vector<const CraftingRecipe*> getAvailableRecipes(Entity* crafter) const;
    std::vector<const CraftingRecipe*> getRecipesByCategory(const std::string& category) const;

    void initializeDefaultRecipes();

    CraftingSystem(const CraftingSystem&) = delete;
    CraftingSystem& operator=(const CraftingSystem&) = delete;
};

#endif // CRAFTINGSYSTEM_H
