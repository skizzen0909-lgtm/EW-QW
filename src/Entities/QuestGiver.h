// Entities/QuestGiver.h
#ifndef QUESTGIVER_H
#define QUESTGIVER_H

#include "Entities/NPC.h"
#include <vector>
#include <string>

struct QuestInfo {
    std::string questId;
    std::string title;
    std::string description;
    bool isActive;
    bool isCompleted;

    QuestInfo(const std::string& id, const std::string& t, const std::string& d)
        : questId(id), title(t), description(d), isActive(false), isCompleted(false) {}
};

class QuestGiver : public NPC {
private:
    std::vector<QuestInfo> availableQuests;
    std::vector<QuestInfo> completedQuests;
    int maxActiveQuests;

public:
    QuestGiver(const std::string& name, int health = 80);
    ~QuestGiver() override = default;

    void interact(Entity* player) override;
    std::string getDialogue() const override;

    void addQuest(const QuestInfo& quest);
    bool giveQuest(const std::string& questId);
    bool completeQuest(const std::string& questId);

    const std::vector<QuestInfo>& getAvailableQuests() const;
    const std::vector<QuestInfo>& getCompletedQuests() const;
    int getMaxActiveQuests() const;

    bool hasQuests() const;
    int getActiveQuestCount() const;

    static std::unique_ptr<QuestGiver> createVillageElder(int level);
    static std::unique_ptr<QuestGiver> createMysteriousStranger(int level);
};

#endif // QUESTGIVER_H
