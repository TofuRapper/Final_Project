#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

struct ScoreEntry {
    std::string name;
    int score;
};

class LeaderboardSystem {
public:
    std::vector<ScoreEntry> entries;

    void load();
    void save();
    void add(std::string name, int score);
};

extern LeaderboardSystem gameLeaderboard;
extern std::vector<ScoreEntry> leaderboard; // Deprecated, use gameLeaderboard.entries


#endif