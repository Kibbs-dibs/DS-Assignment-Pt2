#include <iostream>
#include <string>
#include <map>
using namespace std;

struct MatchLog {
    string stage;
    string teamA;
    string teamB;
    string score;
    string winner;
};

MatchLog matchHistory[200];
int matchCount = 0;
map<string, int> teamWins;

void logMatch(string stage, string teamA, string teamB, string score, string winner) {
    MatchLog log;
    log.stage = stage;
    log.teamA = teamA;
    log.teamB = teamB;
    log.score = score;
    log.winner = winner;
    matchHistory[matchCount++] = log;
    teamWins[winner]++;
}

void viewMatchHistory() {
    cout << "\n=== Match History ===\n";
    for (int i = 0; i < matchCount; i++) {
        cout << "[" << matchHistory[i].stage << "] ";
        cout << matchHistory[i].teamA << " vs " << matchHistory[i].teamB;
        cout << " " << matchHistory[i].score << " -> Winner: ";
        cout << matchHistory[i].winner << "\n";
    }
}

void viewTeamPerformance() {
    cout << "\n=== Team Win Summary ===\n";
    for (auto &entry : teamWins) {
        cout << entry.first << " -> " << entry.second << " wins\n";
    }
}
void performanceMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu: Match Logs & Performance ===\n";
        cout << "1. View Match History\n";
        cout << "2. View Team Performance Summary\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> choice;
        switch (choice) {
            case 1: viewMatchHistory(); break;
            case 2: viewTeamPerformance(); break;
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void simulateTestMatches() {
    logMatch("Qualifiers", "Team Alpha", "Team Beta", "(2-1)", "Team Alpha");
    logMatch("Group A", "Team Alpha", "Team Gamma", "(0-2)", "Team Gamma");
    logMatch("Quarterfinal", "Team Gamma", "Team Delta", "(2-0)", "Team Gamma");
    logMatch("Semifinal", "Team Gamma", "Team Omega", "(2-1)", "Team Gamma");
}

int main() {
    simulateTestMatches();

    int choice;
    do {
        cout << "\n=== Admin Menu: Match Logs & Performance ===\n";
        cout << "1. View Match History\n";
        cout << "2. View Team Performance Summary\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: viewMatchHistory(); break;
            case 2: viewTeamPerformance(); break;
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}
