#include "Leaderboard.h"

bool compareScores(const ScoreEntry& a, const ScoreEntry& b) {
    return a.score > b.score; // Descending order
}

void LeaderboardSystem::load() {
    entries.clear();
    std::ifstream file("leaderboard.txt");
    if (file.is_open()) {
        std::string name;
        int score;
        while (file >> name >> score) {
            entries.push_back({name, score});
        }
        file.close();
    }
    // Sort just in case
    std::sort(entries.begin(), entries.end(), compareScores);
}

void LeaderboardSystem::save() {
    std::ofstream file("leaderboard.txt");
    if (file.is_open()) {
        for (const auto& entry : entries) {
            file << entry.name << " " << entry.score << "\n";
        }
        file.close();
    }
}

void LeaderboardSystem::add(std::string name, int score) {
    entries.push_back({name, score});
    std::sort(entries.begin(), entries.end(), compareScores);
    
    // Keep only top 10
    if (entries.size() > 10) {
        entries.resize(10);
    }
    
    save();
}
