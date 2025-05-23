#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct MatchResult {
    string team1;
    string team2;
    string winner;
    string score;
};

vector<MatchResult> matchHistory;

void logMatch(const string& team1, const string& team2, const string& winner, const string& score) {
    MatchResult result = {team1, team2, winner, score};
    matchHistory.push_back(result);

    ofstream file("match_history.txt", ios::app);
    file << team1 << "," << team2 << "," << winner << "," << score << "\n";
    file.close();

    cout << "Match logged: " << team1 << " vs " << team2 << " - Winner: " << winner << " " << score << endl;
}

void displayMatchHistory() {
    cout << "\n=== Match History ===\n";
    ifstream file("match_history.txt");
    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
}

int main() {
    logMatch("Team Alpha", "Team Beta", "Team Alpha", "2-1");
    logMatch("Team Gamma", "Team Delta", "Team Delta", "3-2");

    displayMatchHistory();
    return 0;
}
