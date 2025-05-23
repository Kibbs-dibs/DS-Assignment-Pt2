#include <iostream>
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
    for (int i = 0; i < 5; i++) sum += rankStrength(t.players[i].rank);
    return sum;
}

Team simulateMatch(Team &a, Team &b, const char *stage, int bestOf, char scoreOutput[10]) {
    int winsA = 0, winsB = 0, needed = (bestOf / 2) + 1;
    int strA = calculateTeamStrength(a), strB = calculateTeamStrength(b), total = strA + strB;
    while (winsA < needed && winsB < needed) {
        int roll = rand() % total;
        if (roll < strA) winsA++; else winsB++;
    }
    sprintf(scoreOutput, "(%d-%d)", winsA, winsB);
    cout << stage << " - Match: " << a.name << " vs " << b.name << " " << scoreOutput << endl;
    cout << "Winner: " << ((winsA > winsB) ? a.name : b.name) << "\n\n";
    return (winsA > winsB) ? a : b;
}

void runQualifiers(Team inTeams[32], Team outQualified[16]) {
    cout << "\n=== QUALIFIERS ROUND (BO1 Single Elimination) ===\n";
    char score[10];
    int q = 0;
    for (int i = 0; i < 32; i += 2) {
        Team winner = simulateMatch(inTeams[i], inTeams[i + 1], "Qualifiers", 1, score);
        outQualified[q++] = winner;
    }
    cout << "\n16 Teams qualified for Group Stage.\n";
}

void runGroupStage(Team group[4], char groupName) {
    cout << "\n--- Group " << groupName << " Matches ---\n";
    for (int i = 0; i < 4; i++) group[i].points = 0;
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            char score[10];
            Team winner = simulateMatch(group[i], group[j], (string("Group ") + groupName).c_str(), 3, score);
            if (strcmp(winner.name, group[i].name) == 0) group[i].points += 3;
            else group[j].points += 3;
        }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3 - i; j++)
            if (group[j].points < group[j + 1].points) {
                Team t = group[j]; group[j] = group[j + 1]; group[j + 1] = t;
            }
    cout << "Group " << groupName << " Standings:\n";
    for (int i = 0; i < 4; i++) {
        cout << "  " << (i + 1) << ". " << group[i].name << " (" << group[i].points << " pts)";
        if (i < 2) cout << "Qualified";
        cout << "\n";
    }
}

Team runPlayoffs(Team top8[8]) {
    const char* rounds[] = {"Quarterfinal", "Semifinal", "Final"};
    char score[10]; Team round[8]; memcpy(round, top8, 8 * sizeof(Team));
    for (int r = 0, size = 8; r < 3; r++) {
        Team next[4]; int idx = 0;
        cout << "--- " << rounds[r] << " ---\n";
        for (int i = 0; i < size; i += 2)
            next[idx++] = simulateMatch(round[i], round[i + 1], rounds[r], 5, score);
        memcpy(round, next, idx * sizeof(Team));
    }
    return round[0];
}

void showChampion(Team &champion) {
    cout << "APU Champion: " << champion.name << "Players:\n";
    Player mvp; int best = 0;
    for (int i = 0; i < 5; i++) {
        cout << "  - " << champion.players[i].name << " (" << champion.players[i].rank << ")\n";
        int r = rankStrength(champion.players[i].rank);
        if (r > best) { best = r; mvp = champion.players[i]; }
    }
    cout << "MVP: " << mvp.name << " (" << mvp.rank << ")\n";
}

void startTournament(Team checkedIn[32]) {
    Team qualified[16];
    runQualifiers(checkedIn, qualified);

    Team groupA[4], groupB[4], groupC[4], groupD[4];
    memcpy(groupA, qualified, 4 * sizeof(Team));
    memcpy(groupB, qualified + 4, 4 * sizeof(Team));
    memcpy(groupC, qualified + 8, 4 * sizeof(Team));
    memcpy(groupD, qualified + 12, 4 * sizeof(Team));

    runGroupStage(groupA, 'A');
    runGroupStage(groupB, 'B');
    runGroupStage(groupC, 'C');
    runGroupStage(groupD, 'D');

    Team top8[8];
    memcpy(top8, groupA, 2 * sizeof(Team));
    memcpy(top8 + 2, groupB, 2 * sizeof(Team));
    memcpy(top8 + 4, groupC, 2 * sizeof(Team));
    memcpy(top8 + 6, groupD, 2 * sizeof(Team));

    Team champion = runPlayoffs(top8);
    showChampion(champion);
}

