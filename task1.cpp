#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Player {
    char name[50];
    char rank[20];
};

struct Team {
    char name[50];
    Player players[5];
    int points = 0;
};

int rankStrength(const char* rank) {
    if (strcmp(rank, "Iron") == 0) return 1;
    if (strcmp(rank, "Bronze") == 0) return 2;
    if (strcmp(rank, "Silver") == 0) return 3;
    if (strcmp(rank, "Gold") == 0) return 4;
    if (strcmp(rank, "Platinum") == 0) return 5;
    if (strcmp(rank, "Diamond") == 0) return 6;
    return 0;
}

int calculateTeamStrength(Team &t) {
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += rankStrength(t.players[i].rank);
    }
    return sum;
}

Team simulateMatch(Team &a, Team &b, const char *stage, int bestOf, char scoreOutput[10]) {
    int winsA = 0, winsB = 0;
    int needed = (bestOf / 2) + 1;

    int strengthA = calculateTeamStrength(a);
    int strengthB = calculateTeamStrength(b);
    int total = strengthA + strengthB;

    while (winsA < needed && winsB < needed) {
        int roll = rand() % total;
        if (roll < strengthA) winsA++;
        else winsB++;
    }

    sprintf(scoreOutput, "(%d-%d)", winsA, winsB);
    cout << stage << " - Match: " << a.name << " vs " << b.name << " " << scoreOutput << endl;

    Team &winner = (winsA > winsB) ? a : b;
    cout << "Winner: " << winner.name << "\n\n";
    return winner;
}

void readCSV(const char *filename, Team teams[32], int &teamCount) {
    ifstream file(filename);
    string line;
    getline(file, line); // skip header
    char lastTeam[50] = "";
    int playerIndex = 0;
    teamCount = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        char team[50], player[50], rank[20];
        int parsed = sscanf(line.c_str(), "%49[^,],%49[^,],%19s", team, player, rank);
        if (parsed < 3) continue;

        if (strcmp(team, lastTeam) != 0) {
            if (teamCount >= 32) break;
            strcpy(teams[teamCount].name, team);
            playerIndex = 0;
            teamCount++;
            strcpy(lastTeam, team);
        }

        if (playerIndex < 5) {
            strcpy(teams[teamCount - 1].players[playerIndex].name, player);
            strcpy(teams[teamCount - 1].players[playerIndex].rank, rank);
            playerIndex++;
        }
    }
}

void runQualifiers(Team teams[32], Team qualified[16]) {
    char score[10];
    int index = 0;
    for (int i = 0; i < 32; i += 2) {
        Team winner = simulateMatch(teams[i], teams[i + 1], "Qualifiers", 1, score);
        qualified[index++] = winner;
    }
}

void sortGroup(Team group[4]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i; j++) {
            if (group[j].points < group[j + 1].points) {
                Team temp = group[j];
                group[j] = group[j + 1];
                group[j + 1] = temp;
            }
        }
    }
}

void runGroupStage(Team group[4], char groupName) {
    char score[10];
    cout << "\n--- Group " << groupName << " Matches ---\n";
    for (int i = 0; i < 4; i++) group[i].points = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            Team winner = simulateMatch(group[i], group[j], (string("Group ") + groupName).c_str(), 3, score);
            if (strcmp(winner.name, group[i].name) == 0) group[i].points += 3;
            else group[j].points += 3;
        }
    }

    // Sort and display full standings
    sortGroup(group);
    cout << "\nGroup " << groupName << " Standings:\n";
    for (int i = 0; i < 4; i++) {
        cout << "  " << (i + 1) << ". " << group[i].name << " (" << group[i].points << " pts)";
        if (i < 2) cout << "Qualified";
        cout << "\n";
    }
    cout << endl;
}

void selectTop2(Team group[4], Team result[2]) {
    result[0] = group[0];
    result[1] = group[1];
}

Team runPlayoffs(Team teams[8]) {
    Team current[8];
    memcpy(current, teams, 8 * sizeof(Team));
    int size = 8;
    const char* stages[] = {"QUATERFINAL", "SEMIFINAL", "GRANDFINAL"};
    int bestOf = 5;
    char score[10];

    for (int round = 0; round < 3; round++) {
        Team next[4];
        int idx = 0;
        cout << "\n--- " << stages[round] << " ---\n";
        for (int i = 0; i < size; i += 2) {
            Team winner = simulateMatch(current[i], current[i + 1], stages[round], bestOf, score);
            next[idx++] = winner;
        }
        memcpy(current, next, idx * sizeof(Team));
        size = idx;
    }
    return current[0];
}

void printChampionPlayers(Team& champion) {
    cout << "\nAPU CHAMPION: " << champion.name << endl;
    cout << "Team Players:\n";

    int bestRankValue = 0;
    Player mvp;

    for (int i = 0; i < 5; i++) {
        int r = rankStrength(champion.players[i].rank);
        cout << "  - " << champion.players[i].name << " (" << champion.players[i].rank << ")\n";
        if (r > bestRankValue) {
            bestRankValue = r;
            mvp = champion.players[i];
        }
    }

    cout << "\nMVP: " << mvp.name << " (" << mvp.rank << ")\n";
}

int main() {
    srand(time(0));

    Team allTeams[32], qualified[16];
    int teamCount = 0;

    readCSV("apuesport.csv", allTeams, teamCount);

    if (teamCount < 32) {
        cout << "❌ Not enough teams in CSV (found " << teamCount << "). You need exactly 32.\n";
        return 1;
    }

    // Step 1: Qualifiers
    cout << "\n--- QUALIFIERS ---\n";
    runQualifiers(allTeams, qualified);

    // Step 2: Group Stage
    Team groupA[4], groupB[4], groupC[4], groupD[4];
    memcpy(groupA, qualified, 4 * sizeof(Team));
    memcpy(groupB, qualified + 4, 4 * sizeof(Team));
    memcpy(groupC, qualified + 8, 4 * sizeof(Team));
    memcpy(groupD, qualified + 12, 4 * sizeof(Team));

    runGroupStage(groupA, 'A');
    runGroupStage(groupB, 'B');
    runGroupStage(groupC, 'C');
    runGroupStage(groupD, 'D');

    // Step 3: Top 8
    Team top8[8], tmp[2];
    selectTop2(groupA, tmp); memcpy(top8, tmp, 2 * sizeof(Team));
    selectTop2(groupB, tmp); memcpy(top8 + 2, tmp, 2 * sizeof(Team));
    selectTop2(groupC, tmp); memcpy(top8 + 4, tmp, 2 * sizeof(Team));
    selectTop2(groupD, tmp); memcpy(top8 + 6, tmp, 2 * sizeof(Team));

    // Step 4: Playoffs
    cout << "\n=== PLAYOFFS ===\n";
    Team champion = runPlayoffs(top8);

    // Final Output
    printChampionPlayers(champion);

    return 0;
}
