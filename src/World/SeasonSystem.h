// World/SeasonSystem.h
#ifndef SEASONSYSTEM_H
#define SEASONSYSTEM_H

#include <string>
#include <unordered_map>
#include <vector>

enum class Season {
    SPRING,
    SUMMER,
    AUTUMN,
    WINTER
};

struct SeasonModifier {
    float temperatureShift;
    float moistureShift;
    float dayLengthMultiplier;
    float encounterRateModifier;
    float resourceAbundance;
    std::string weatherPattern;

    SeasonModifier()
        : temperatureShift(0), moistureShift(0), dayLengthMultiplier(1.0f),
          encounterRateModifier(1.0f), resourceAbundance(1.0f) {}
};

class SeasonSystem {
private:
    Season currentSeason;
    float seasonTimer;
    float seasonDuration;
    float timeAccumulated;

    std::unordered_map<Season, SeasonModifier> seasonModifiers;
    std::unordered_map<Season, std::string> seasonNames;

    void initializeSeasonData();

public:
    SeasonSystem();
    ~SeasonSystem() = default;

    void update(float deltaTime);

    Season getCurrentSeason() const;
    std::string getCurrentSeasonName() const;
    const SeasonModifier& getCurrentModifiers() const;

    void setSeason(Season season);
    void advanceSeason();
    float getSeasonProgress() const;
    float getTimeUntilNextSeason() const;

    Season getSeasonByIndex(int index) const;

    SeasonSystem(const SeasonSystem&) = delete;
    SeasonSystem& operator=(const SeasonSystem&) = delete;
};

#endif // SEASONSYSTEM_H
