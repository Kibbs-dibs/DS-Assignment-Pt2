#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <map>
using namespace std;

struct Player {
    char name[50];
    char rank[20];
};

struct Team {
    char name[50];
    Player players[5];
    char regType[10];
    int points = 0;
};

Team registrationQueue[64], checkedIn[32];
int regCount = 0, checkedCount = 0;

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

bool isTeamAlreadyCheckedIn(const char* teamName) {
    for (int i = 0; i < checkedCount; i++)
        if (strcmp(checkedIn[i].name, teamName) == 0)
            return true;
    return false;
}

void readRegistrations() {
    ifstream file("apuesport(2.0).csv");
    string line;
    getline(file, line);
    map<string, int> playerIndex;
    char team[50], player[50], rank[20], reg[10];
    int index = -1;

    while (getline(file, line)) {
        sscanf(line.c_str(), "%[^,],%[^,],%[^,],%s", team, player, rank, reg);
        if (playerIndex.find(team) == playerIndex.end()) {
            index++;
            strcpy(registrationQueue[index].name, team);
            strcpy(registrationQueue[index].regType, reg);
            playerIndex[team] = 0;
            regCount++;
        }
        int i = playerIndex[team];
        strcpy(registrationQueue[index].players[i].name, player);
        strcpy(registrationQueue[index].players[i].rank, rank);
        playerIndex[team]++;
    }
}

void viewCheckedInTeams() {
    cout << "\n=== Checked-In Teams ===\n";
    for (int i = 0; i < checkedCount; i++)
        cout << i + 1 << ". " << checkedIn[i].name << " (" << checkedIn[i].regType << ")\n";
    cout << "Total: " << checkedCount << "/32 teams\n";
}

void adminSingleCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full (32 teams).\n";
        return;
    }

    cout << "\n=== Admin: Single Team Check-In ===\n";
    char name[50];
    cout << "Enter team name to check in: ";
    cin.ignore();
    cin.getline(name, 50);

    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].name, name) == 0) {
            if (isTeamAlreadyCheckedIn(name)) {
                cout << "Team is already checked in.\n";
            } else {
                checkedIn[checkedCount++] = registrationQueue[i];
                cout << name << " has been checked in.\n";
            }
            return;
        }
    }

    cout << "Team not found in registration file.\n";
}

void withdrawAndReplace() {
    cout << "\n--- Withdraw and Replace ---\n";
    if (checkedCount == 0) {
        cout << "No teams checked in yet.\n";
        return;
    }

    viewCheckedInTeams();
    cout << "Enter team name to withdraw: ";
    char target[50];
    cin.ignore(); cin.getline(target, 50);

    int found = -1;
    for (int i = 0; i < checkedCount; i++) {
        if (strcmp(checkedIn[i].name, target) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        cout << "Team not found.\n";
        return;
    }

    bool replaced = false;
    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].regType, "wildcard") == 0 &&
            !isTeamAlreadyCheckedIn(registrationQueue[i].name)) {
            cout << "Replacing with wildcard team: " << registrationQueue[i].name << "\n";
            checkedIn[found] = registrationQueue[i];
            replaced = true;
            break;
        }
    }

    if (!replaced) {
        for (int i = found; i < checkedCount - 1; i++)
            checkedIn[i] = checkedIn[i + 1];
        checkedCount--;
        cout << "No wildcard teams left. Slot removed.\n";
    } else {
        cout << "Replacement successful.\n";
    }

    viewCheckedInTeams();
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
    cout << "\nGroup " << groupName << " Standings:\n";
    for (int i = 0; i < 4; i++) {
        cout << "  " << (i + 1) << ". " << group[i].name << " (" << group[i].points << " pts)";
        if (i < 2) cout << "Qualified"; cout << "\n";
    }
}

Team runPlayoffs(Team top8[8]) {
    const char* rounds[] = {"Quarterfinal", "Semifinal", "Final"};
    char score[10]; Team round[8]; memcpy(round, top8, 8 * sizeof(Team));
    for (int r = 0, size = 8; r < 3; r++) {
        Team next[4]; int idx = 0;
        cout << "\n--- " << rounds[r] << " ---\n";
        for (int i = 0; i < size; i += 2)
            next[idx++] = simulateMatch(round[i], round[i + 1], rounds[r], 5, score);
        memcpy(round, next, idx * sizeof(Team));
    }
    return round[0];
}

void showChampion(Team &champion) {
    cout << "\nAPU Champion: " << champion.name << "\nTeam Players:\n";
    Player mvp; int best = 0;
    for (int i = 0; i < 5; i++) {
        cout << "  - " << champion.players[i].name << " (" << champion.players[i].rank << ")\n";
        int r = rankStrength(champion.players[i].rank);
        if (r > best) { best = r; mvp = champion.players[i]; }
    }
    cout << " MVP: " << mvp.name << " (" << mvp.rank << ")\n";
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

void startTournament() {
    Team groupA[4], groupB[4], groupC[4], groupD[4];
    memcpy(groupA, checkedIn, 4 * sizeof(Team));
    memcpy(groupB, checkedIn + 4, 4 * sizeof(Team));
    memcpy(groupC, checkedIn + 8, 4 * sizeof(Team));
    memcpy(groupD, checkedIn + 12, 4 * sizeof(Team));
    runGroupStage(groupA, 'A'); runGroupStage(groupB, 'B');
    runGroupStage(groupC, 'C'); runGroupStage(groupD, 'D');

    Team top8[8];
    memcpy(top8, groupA, 2 * sizeof(Team));
    memcpy(top8 + 2, groupB, 2 * sizeof(Team));
    memcpy(top8 + 4, groupC, 2 * sizeof(Team));
    memcpy(top8 + 6, groupD, 2 * sizeof(Team));

    Team champion = runPlayoffs(top8);
    showChampion(champion);
}

void adminMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Last-Minute Check-In Team\n";
        cout << "2. Withdraw and replace a team\n";
        cout << "3. View checked-in teams\n";
        cout << "4. Start tournament\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: adminSingleCheckIn(); break;
            case 2: withdrawAndReplace(); break;
            case 3: viewCheckedInTeams(); break;
            case 4:
                if (checkedCount == 32) {
                    cout << "\n32 teams ready. Starting tournament...\n";
                    startTournament();
                    return;
                } else {
                    cout << "Not enough teams. " << (32 - checkedCount) << " slot(s) left.\n";
                }
                break;
            case 0: cout << "Returning to login menu.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

void processTeamCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full.\n"; return;
    }
    char name[50];
    cout << "Enter your team name: ";
    cin.ignore(); cin.getline(name, 50);
    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].name, name) == 0) {
            if (isTeamAlreadyCheckedIn(name)) {
                cout << "Already checked in.\n"; return;
            }
            checkedIn[checkedCount++] = registrationQueue[i];
            cout << name << " checked in successfully.\n";
            viewCheckedInTeams();
            if (checkedCount == 32) {
                cout << "\nAuto-starting tournament...\n";
                startTournament();
            }
            return;
        }
    }
    cout << "Team not found in registration list.\n";
}

int main() {
    srand(time(0));
    readRegistrations();

    while (true) {
        char access;
        cout << "\nLogin as (A)dmin or (T)eam or (Q)uit: ";
        cin >> access;

        if (access == 'A' || access == 'a') {
            adminMenu();
        } else if (access == 'T' || access == 't') {
            processTeamCheckIn();
        } else if (access == 'Q' || access == 'q') {
            cout << "Exiting program. Goodbye!\n";
            break;
        } else {
            cout << "Invalid input. Try again.\n";
        }
    }

    return 0;
}
