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
        (roll < strA) ? winsA++ : winsB++;
    }
    sprintf(scoreOutput, "(%d-%d)", winsA, winsB);
    cout << stage << " - Match: " << a.name << " vs " << b.name << " " << scoreOutput << endl;
    return (winsA > winsB) ? a : b;
}

void waitForUser(string stageName) {
    string input;
    cout << "\nType 'proceed' to continue to " << stageName << " or any other key to pause: ";
    cin >> input;
    if (input != "proceed") {
        cout << stageName << " paused.\n";
        exit(0);
    }
}

Team runDoubleElimination(Team top8[8]) {
    char score[10];

    // WB Quarterfinals
    Team wbQWinners[4];
    for (int i = 0; i < 4; i++)
        wbQWinners[i] = simulateMatch(top8[i * 2], top8[i * 2 + 1], "WB Quarterfinal", 3, score);
    waitForUser("LB Round 1");

    // LB Round 1: Losers of WB QF
    Team lbR1[4]; int lbIdx = 0;
    for (int i = 0; i < 4; i++) {
        if (strcmp(wbQWinners[i].name, top8[i * 2].name) == 0)
            lbR1[lbIdx++] = top8[i * 2 + 1];
        else
            lbR1[lbIdx++] = top8[i * 2];
    }
    Team lbR2[2];
    lbR2[0] = simulateMatch(lbR1[0], lbR1[1], "LB Round 1", 3, score);
    lbR2[1] = simulateMatch(lbR1[2], lbR1[3], "LB Round 1", 3, score);
    waitForUser("WB Semifinal");

    // WB Semifinals
    Team wbSemiWinners[2];
    Team wbSemiLosers[2];
    for (int i = 0; i < 2; i++) {
        Team winner = simulateMatch(wbQWinners[i * 2], wbQWinners[i * 2 + 1], "WB Semifinal", 3, score);
        wbSemiWinners[i] = winner;
        wbSemiLosers[i] = (strcmp(winner.name, wbQWinners[i * 2].name) == 0) ? wbQWinners[i * 2 + 1] : wbQWinners[i * 2];
    }
    waitForUser("LB Round 2");
    
    // LB Round 2
    Team lbR3[2];
    lbR3[0] = simulateMatch(lbR2[0], wbSemiLosers[0], "LB Round 2", 3, score);
    lbR3[1] = simulateMatch(lbR2[1], wbSemiLosers[1], "LB Round 2", 3, score);
    waitForUser("LB Final Qualifier");

    // LB Final Qualifier
    Team lbFinalQualifier = simulateMatch(lbR3[0], lbR3[1], "LB Final Qualifier", 3, score);
    waitForUser("WB Final");

    // WB Final
    Team wbFinalWinner = simulateMatch(wbSemiWinners[0], wbSemiWinners[1], "WB Final", 3, score);
    Team wbFinalLoser = (strcmp(wbFinalWinner.name, wbSemiWinners[0].name) == 0) ? wbSemiWinners[1] : wbSemiWinners[0];
    waitForUser("LB Final");

    // LB Final
    Team lbFinal = simulateMatch(lbFinalQualifier, wbFinalLoser, "LB Final", 5, score);
    waitForUser("Grand Final");

    // Grand Final
    Team champion = simulateMatch(wbFinalWinner, lbFinal, "Grand Final", 5, score);
    return champion;
}

void showChampion(Team &champion) {
    cout << "\nAPU Champion: " << champion.name << "\n Team Players:\n";
    Player mvp; int best = 0;
    for (int i = 0; i < 5; i++) {
        cout << "  - " << champion.players[i].name << " (" << champion.players[i].rank << ")\n";
        int r = rankStrength(champion.players[i].rank);
        if (r > best) { best = r; mvp = champion.players[i]; }
    }
    cout << "MVP: " << mvp.name << " (" << mvp.rank << ")\n";
}

void startTournament(Team checkedIn[32]) {
    string input;
    Team qualified[16];
    cout << "\n=== QUALIFIERS ROUND ===\n";
    char score[10];
    int q = 0;
    for (int i = 0; i < 32; i += 2)
        qualified[q++] = simulateMatch(checkedIn[i], checkedIn[i + 1], "Qualifiers", 1, score);

    cout << "\nQualifiers completed.\n";
    cout << "Type 'proceed' to continue to Group Stage or any other key to cancel: ";
    cin >> input;
    if (input != "proceed") {
        cout << "Tournament paused after Qualifiers.\n";
        return;
    }

    Team groupA[4], groupB[4], groupC[4], groupD[4];
    memcpy(groupA, qualified, 4 * sizeof(Team));
    memcpy(groupB, qualified + 4, 4 * sizeof(Team));
    memcpy(groupC, qualified + 8, 4 * sizeof(Team));
    memcpy(groupD, qualified + 12, 4 * sizeof(Team));

    runGroupStage(groupA, 'A');
    runGroupStage(groupB, 'B');
    runGroupStage(groupC, 'C');
    runGroupStage(groupD, 'D');

    cout << "\nGroup Stage completed.\n";
    cout << "Type 'proceed' to continue to Playoffs (Double Elimination) or any other key to cancel: ";
    cin >> input;
    if (input != "proceed") {
        cout << "Tournament paused after Group Stage.\n";
        return;
    }

    Team top8[8]; int idx = 0;
    for (int g = 0; g < 4; g++) {
        Team* group = (g == 0 ? groupA : g == 1 ? groupB : g == 2 ? groupC : groupD);
        for (int i = 0; i < 4; i++)
            if (group[i].points == 3)
                top8[idx++] = group[i];
    }

    Team champion = runDoubleElimination(top8);
    showChampion(champion);
}

void runGroupStage(Team group[4], char groupName) {
    cout << "\n--- Group " << groupName << " Matches (Bracket Style) ---\n";
    Team winner1 = simulateMatch(group[0], group[1], "Opening Match", 3, new char[10]);
    Team loser1 = (strcmp(winner1.name, group[0].name) == 0) ? group[1] : group[0];
    Team winner2 = simulateMatch(group[2], group[3], "Opening Match", 3, new char[10]);
    Team loser2 = (strcmp(winner2.name, group[2].name) == 0) ? group[3] : group[2];
    Team firstPlace = simulateMatch(winner1, winner2, "Winners Match", 3, new char[10]);
    Team runnerUp = (strcmp(firstPlace.name, winner1.name) == 0) ? winner2 : winner1;
    Team survivor = simulateMatch(loser1, loser2, "Elimination Match", 3, new char[10]);
    Team secondPlace = simulateMatch(runnerUp, survivor, "Decider Match", 3, new char[10]);

    cout << "\n--- Group " << groupName << " Final Standings ---\n";
    cout << "1. " << firstPlace.name << " Qualified\n";
    cout << "2. " << secondPlace.name << " Qualified\n";

    for (int i = 0; i < 4; i++)
        group[i].points = (strcmp(group[i].name, firstPlace.name) == 0 || strcmp(group[i].name, secondPlace.name) == 0) ? 3 : 0;
}

int main() {
    srand(time(0));
    Team dummyCheckedIn[32]; // Fill with test data or link with Task 2
    // TODO: Populate dummyCheckedIn with 32 valid teams if testing standalone
    startTournament(dummyCheckedIn);
    return 0;
}
