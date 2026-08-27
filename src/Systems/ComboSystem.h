// Systems/ComboSystem.h
#ifndef COMBOSYSTEM_H
#define COMBOSYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

class Entity;

struct ComboStep {
    std::string abilityId;
    std::string name;
    int damage;
    float timeWindow;
    bool executed;
};

struct ComboSequence {
    std::string id;
    std::string name;
    std::string description;
    std::vector<ComboStep> steps;
    int totalDamage;
    float overallTimeLimit;
    bool completed;

    ComboSequence() : totalDamage(0), overallTimeLimit(3.0f), completed(false) {}
};

class ComboSystem {
private:
    std::unordered_map<std::string, ComboSequence> registeredCombos;
    std::vector<ComboStep> currentComboBuffer;
    float comboTimer;
    float comboTimeLimit;
    int comboCount;

    bool isSequenceMatched(const std::vector<ComboStep>& buffer, const ComboSequence& sequence) const;

public:
    ComboSystem();
    ~ComboSystem() = default;

    void registerCombo(const ComboSequence& combo);
    void unregisterCombo(const std::string& comboId);

    void onAbilityUsed(const std::string& abilityId, Entity* caster, Entity* target);
    void update(float deltaTime);

    bool isComboActive() const;
    int getComboCount() const;
    std::string getCurrentComboName() const;
    float getComboTimer() const;
    void resetCombo();

    const std::unordered_map<std::string, ComboSequence>& getRegisteredCombos() const;
    void initializeDefaultCombos();

    ComboSystem(const ComboSystem&) = delete;
    ComboSystem& operator=(const ComboSystem&) = delete;
};

#endif // COMBOSYSTEM_H
