// World/SeasonSystem.cpp
#include "SeasonSystem.h"
#include <iostream>
#include <sstream>

SeasonSystem::SeasonSystem()
    : currentSeason(Season::SPRING)
    , seasonTimer(0.0f)
    , seasonDuration(300.0f)
    , timeAccumulated(0.0f) {
    initializeSeasonData();
}

void SeasonSystem::initializeSeasonData() {
    seasonNames[Season::SPRING] = "Весна";
    seasonNames[Season::SUMMER] = "Лето";
    seasonNames[Season::AUTUMN] = "Осень";
    seasonNames[Season::WINTER] = "Зима";

    SeasonModifier spring;
    spring.temperatureShift = 0.0f;
    spring.moistureShift = 0.2f;
    spring.dayLengthMultiplier = 1.0f;
    spring.encounterRateModifier = 1.0f;
    spring.resourceAbundance = 1.2f;
    spring.weatherPattern = "Дождь, тёплый ветер";
    seasonModifiers[Season::SPRING] = spring;

    SeasonModifier summer;
    summer.temperatureShift = 0.3f;
    summer.moistureShift = -0.1f;
    summer.dayLengthMultiplier = 1.3f;
    summer.encounterRateModifier = 1.1f;
    summer.resourceAbundance = 1.5f;
    summer.weatherPattern = "Ясно, жарко";
    seasonModifiers[Season::SUMMER] = summer;

    SeasonModifier autumn;
    autumn.temperatureShift = -0.2f;
    autumn.moistureShift = 0.1f;
    autumn.dayLengthMultiplier = 0.9f;
    autumn.encounterRateModifier = 0.9f;
    autumn.resourceAbundance = 0.8f;
    autumn.weatherPattern = "Ветер, дождь, туман";
    seasonModifiers[Season::AUTUMN] = autumn;

    SeasonModifier winter;
    winter.temperatureShift = -0.5f;
    winter.moistureShift = 0.3f;
    winter.dayLengthMultiplier = 0.7f;
    winter.encounterRateModifier = 0.7f;
    winter.resourceAbundance = 0.5f;
    winter.weatherPattern = "Снег, мороз";
    seasonModifiers[Season::WINTER] = winter;
}

void SeasonSystem::update(float deltaTime) {
    timeAccumulated += deltaTime;
    seasonTimer += deltaTime;

    if (seasonTimer >= seasonDuration) {
        seasonTimer = 0.0f;
        advanceSeason();
    }
}

Season SeasonSystem::getCurrentSeason() const {
    return currentSeason;
}

std::string SeasonSystem::getCurrentSeasonName() const {
    auto it = seasonNames.find(currentSeason);
    return (it != seasonNames.end()) ? it->second : "Неизвестно";
}

const SeasonModifier& SeasonSystem::getCurrentModifiers() const {
    auto it = seasonModifiers.find(currentSeason);
    static SeasonModifier defaultMod;
    return (it != seasonModifiers.end()) ? it->second : defaultMod;
}

void SeasonSystem::setSeason(Season season) {
    currentSeason = season;
    seasonTimer = 0.0f;

    std::ostringstream oss;
    oss << "[SEASON] Наступает " << getCurrentSeasonName();
    std::cout << oss.str() << std::endl;
}

void SeasonSystem::advanceSeason() {
    int index = static_cast<int>(currentSeason);
    index = (index + 1) % 4;
    currentSeason = static_cast<Season>(index);
    seasonTimer = 0.0f;

    std::ostringstream oss;
    oss << "[SEASON] Наступает " << getCurrentSeasonName() << "!";
    std::cout << oss.str() << std::endl;
}

float SeasonSystem::getSeasonProgress() const {
    return seasonTimer / seasonDuration;
}

float SeasonSystem::getTimeUntilNextSeason() const {
    return seasonDuration - seasonTimer;
}

Season SeasonSystem::getSeasonByIndex(int index) const {
    return static_cast<Season>(index % 4);
}
