#include <iostream>
#include <string>
#include <map>
using namespace std;

struct MatchLog {
    string stage;
    string group;  // Group A, B, C, D or empty
    string teamA;
    string teamB;
    string score;
    string winner;
};

MatchLog matchHistory[200];
int matchCount = 0;
map<string, int> teamWins;
int sessionMatchStartIndex = 0;

void logMatch(string stage, string teamA, string teamB, string score, string winner, string group = "") {
    MatchLog log;
    log.stage = stage;
    log.group = group;
    log.teamA = teamA;
    log.teamB = teamB;
    log.score = score;
    log.winner = winner;
    matchHistory[matchCount++] = log;
    teamWins[winner]++;
}

void viewMatchHistory() {
    cout << "\n=== Match History (Current Session) ===\n";
    for (int i = sessionMatchStartIndex; i < matchCount; i++) {
        string stage = matchHistory[i].stage;
        string group = matchHistory[i].group;

        if (!group.empty()) {
            stage = group + " - " + stage;
        }

        cout << "[" << stage << "] ";
        cout << matchHistory[i].teamA << " vs " << matchHistory[i].teamB;
        cout << " " << matchHistory[i].score << " -> Winner: ";
        cout << matchHistory[i].winner << "\n";
    }
}

void viewTeamPerformance() {
    cout << "\n=== Team Win Summary (Current Session) ===\n";
    map<string, int> currentWins;
    for (int i = sessionMatchStartIndex; i < matchCount; i++) {
        currentWins[matchHistory[i].winner]++;
    }
    for (auto &entry : currentWins) {
        cout << entry.first << " -> " << entry.second << " wins\n";
    }
}


void tournamentAnalysis() {
    cout << "\n=== Tournament Analysis: Access Past Results ===\n";
    cout << "Total Matches Logged: " << matchCount << "\n";
    viewMatchHistory();
    viewTeamPerformance();
}

void performanceMenu() {
    int opt;
    do {
        cout << "\n=== Performance & Analysis Menu ===\n";
        cout << "1. View Match History\n";
        cout << "2. View Team Performance\n";
        cout << "3. Full Tournament Analysis\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> opt;
        switch (opt) {
            case 1: viewMatchHistory(); break;
            case 2: viewTeamPerformance(); break;
            case 3: tournamentAnalysis(); break;
            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (opt != 0);
}

void simulateTestMatches() {
    logMatch("Opening Match", "Team Alpha", "Team Beta", "(2-1)", "Team Alpha", "Group A");
    logMatch("Winners Match", "Team Alpha", "Team Gamma", "(0-2)", "Team Gamma", "Group A");
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
