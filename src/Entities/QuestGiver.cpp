// Entities/QuestGiver.cpp
#include "QuestGiver.h"
#include <iostream>

QuestGiver::QuestGiver(const std::string& name, int health)
    : NPC(name, "quest_giver", health)
    , maxActiveQuests(3) {
}

void QuestGiver::interact(Entity* player) {
    NPC::interact(player);
    if (hasQuests()) {
        std::cout << "[QUEST_GIVER] " << getName() << ": У меня есть задания!" << std::endl;
    } else {
        std::cout << "[QUEST_GIVER] " << getName() << ": Возвращайся позже!" << std::endl;
    }
}

std::string QuestGiver::getDialogue() const {
    if (hasQuests()) return "Герой, у меня есть важное дело!";
    return "Спасибо за помощь!";
}

void QuestGiver::addQuest(const QuestInfo& quest) {
    availableQuests.push_back(quest);
}

bool QuestGiver::giveQuest(const std::string& questId) {
    if (getActiveQuestCount() >= maxActiveQuests) return false;
    for (auto& quest : availableQuests) {
        if (quest.questId == questId && !quest.isActive && !quest.isCompleted) {
            quest.isActive = true;
            return true;
        }
    }
    return false;
}

bool QuestGiver::completeQuest(const std::string& questId) {
    for (auto& quest : availableQuests) {
        if (quest.questId == questId && quest.isActive) {
            quest.isActive = false;
            quest.isCompleted = true;
            completedQuests.push_back(quest);
            return true;
        }
    }
    return false;
}

const std::vector<QuestInfo>& QuestGiver::getAvailableQuests() const { return availableQuests; }
const std::vector<QuestInfo>& QuestGiver::getCompletedQuests() const { return completedQuests; }
int QuestGiver::getMaxActiveQuests() const { return maxActiveQuests; }

bool QuestGiver::hasQuests() const {
    for (const auto& quest : availableQuests) {
        if (!quest.isActive && !quest.isCompleted) return true;
    }
    return false;
}

int QuestGiver::getActiveQuestCount() const {
    int count = 0;
    for (const auto& quest : availableQuests) {
        if (quest.isActive) count++;
    }
    return count;
}

std::unique_ptr<QuestGiver> QuestGiver::createVillageElder(int level) {
    auto elder = std::make_unique<QuestGiver>("Староста", 100);
    elder->addQuest(QuestInfo("quest_wolf_hunt", "Охота на волков",
        "Убей 5 волков, которые пугают деревню"));
    elder->addQuest(QuestInfo("quest_find_herbs", "Поиск трав",
        "Найди целебные травы для лекаря"));
    elder->addQuest(QuestInfo("quest_clear_cave", "Очисти пещеру",
        "Прочисти пещеру от скелетов"));
    return elder;
}

std::unique_ptr<QuestGiver> QuestGiver::createMysteriousStranger(int level) {
    auto stranger = std::make_unique<QuestGiver>("Таинственный незнакомец", 60);
    stranger->addQuest(QuestInfo("quest_dark_artifact", "Тёмный артефакт",
        "Найди древний артефакт в заброшенной шахте"));
    stranger->addQuest(QuestInfo("quest_relic_hunt", "Охота за реликвией",
        "Принеси мне древний свиток из руин"));
    return stranger;
}
