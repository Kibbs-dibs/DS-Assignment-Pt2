#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <queue>
using namespace std;

struct Team;
int calculateTeamStrength(Team &t);
void logMatch(string stage, string teamA, string teamB, string score, string winner);
void performanceMenu();

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

struct Spectator {
    string name;
    string type; 
};

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
Team registrationQueue[64], checkedIn[32];
int regCount = 0, checkedCount = 0;

queue<Spectator> vipQueue;
queue<Spectator> streamerQueue;
queue<Spectator> generalQueue;
Spectator assignedVIPs[100]; int vipCount = 0;
Spectator assignedGenerals[100]; int generalCount = 0;
Spectator assignedStreamers[100]; int streamCount = 0;

bool adminLogin() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";
    cin >> password;
    return (username == "admin" && password == "1234");
}

int rankStrength(const char* rank) {
    if (strcmp(rank, "Iron") == 0) return 1;
    if (strcmp(rank, "Bronze") == 0) return 2;
    if (strcmp(rank, "Silver") == 0) return 3;
    if (strcmp(rank, "Gold") == 0) return 4;
    if (strcmp(rank, "Platinum") == 0) return 5;
    if (strcmp(rank, "Diamond") == 0) return 6;
    return 0;
}

int calculateTeamStrength(Team &t);
void startTournament(); 

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
    ifstream file("apuesport(40teams).csv");
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

    char name[50];
    cout << "Enter team name to check in: ";
    cin.ignore();
    cin.getline(name, 50);

    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].name, name) == 0) {
            if (!isTeamAlreadyCheckedIn(name)) {
                // Ask for priority level
                string priority;
                cout << "Enter registration type (earlybird/wildcard): ";
                cin >> priority;
                if (priority == "earlybird")
                    strcpy(registrationQueue[i].regType, "earlybird");
                else if (priority == "wildcard")
                    strcpy(registrationQueue[i].regType, "wildcard");
                else {
                    cout << "Invalid type. Defaulting to general.\n";
                    strcpy(registrationQueue[i].regType, "general");
                }

                checkedIn[checkedCount++] = registrationQueue[i];
                cout << name << " has been checked in.\n";
            } else cout << "Team is already checked in.\n";
            return;
        }
    }
    cout << "Team not found in registration file.\n";
}

void withdrawAndReplace() {
    viewCheckedInTeams();
    cout << "Enter team name to withdraw: ";
    char target[50];
    cin.ignore(); cin.getline(target, 50);
    int found = -1;
    for (int i = 0; i < checkedCount; i++) {
        if (strcmp(checkedIn[i].name, target) == 0) {
            found = i; break;
        }
    }
    if (found == -1) { cout << "Team not found.\n"; return; }
    bool replaced = false;
    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].regType, "wildcard") == 0 &&
            !isTeamAlreadyCheckedIn(registrationQueue[i].name)) {
            cout << "Replacing with wildcard team: " << registrationQueue[i].name << "\n";
            checkedIn[found] = registrationQueue[i]; replaced = true; break;
        }
    }
    if (!replaced) {
        for (int i = found; i < checkedCount - 1; i++) checkedIn[i] = checkedIn[i + 1];
        checkedCount--; cout << "No wildcard teams left. Slot removed.\n";
    } else cout << "Replacement successful.\n";
    viewCheckedInTeams();
}

void processTeamCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full.\n";
        return;
    }
    char name[50];
    cout << "Enter your team name: ";
    cin.ignore();
    cin.getline(name, 50);
    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].name, name) == 0) {
            if (!isTeamAlreadyCheckedIn(name)) {
                // Ask for priority level
                string priority;
                cout << "Enter registration type (earlybird/wildcard): ";
                cin >> priority;
                if (priority == "earlybird")
                    strcpy(registrationQueue[i].regType, "earlybird");
                else if (priority == "wildcard")
                    strcpy(registrationQueue[i].regType, "wildcard");
                else {
                    cout << "Invalid type. Defaulting to general.\n";
                    strcpy(registrationQueue[i].regType, "general");
                }

                checkedIn[checkedCount++] = registrationQueue[i];
                cout << name << " checked in successfully.\n";
                viewCheckedInTeams();

                if (checkedCount == 32) {
                    cout << "\nAll 32 teams have been registered.\n";
                    cout << "Waiting for Admin to start the tournament.\n";
                }
            } else {
                cout << "Already checked in.\n";
            }
            return;
        }
    }
    cout << "Team not found in registration list.\n";
}

void spectatorMenu();
void adminMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Last-Minute Check-In Team\n";
        cout << "2. Withdraw and replace a team\n";
        cout << "3. View checked-in teams\n";
        cout << "4. Spectator Management\n";
        cout << "5. Start Tournament\n";
        cout << "6. Match Logs & Team Performance\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        switch (choice) {
            case 1: adminSingleCheckIn(); break;
            case 2: withdrawAndReplace(); break;
            case 3: viewCheckedInTeams(); break;
            case 4: spectatorMenu(); break;
            case 5: startTournament(); break;
            case 6: performanceMenu(); break;
            case 0: cout << "Returning to login menu.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);
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
    string winnerName = (winsA > winsB) ? a.name : b.name;
    logMatch(stage, a.name, b.name, scoreOutput, winnerName);
    return (winsA > winsB) ? a : b;
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

Team runPlayoffs(Team top8[8]) {
    const char* rounds[] = {"Quarterfinal", "Semifinal", "Final"};
    Team round[8]; memcpy(round, top8, 8 * sizeof(Team));
    char score[10];
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
    cout << "\nAPU Champion: " << champion.name << "\n Team Players:\n";
    Player mvp; int best = 0;
    for (int i = 0; i < 5; i++) {
        cout << "  - " << champion.players[i].name << " (" << champion.players[i].rank << ")\n";
        int r = rankStrength(champion.players[i].rank);
        if (r > best) { best = r; mvp = champion.players[i]; }
    }
    cout << "MVP: " << mvp.name << " (" << mvp.rank << ")\n";
}

void startTournament() {
        if (checkedCount < 32) {
        cout << "\nCannot start tournament. Only " << checkedCount << " teams registered. Need 32.\n";
        return;
    }
    string input;
    Team qualified[16];
    teamWins.clear();     
    matchCount = 0;
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

    Team top8[8]; int idx = 0;
    for (int g = 0; g < 4; g++) {
        Team* group = (g == 0 ? groupA : g == 1 ? groupB : g == 2 ? groupC : groupD);
        for (int i = 0; i < 4; i++)
            if (group[i].points == 3)
                top8[idx++] = group[i];
    }

    cout << "\nGroup Stage completed.\n";
    cout << "Type 'proceed' to continue to Playoffs (Quarterfinals) or any other key to cancel: ";
    cin >> input;
    if (input != "proceed") {
        cout << "Tournament paused after Group Stage.\n";
        return;
    }

    const char* rounds[] = {"Quarterfinal", "Semifinal", "Final"};
    Team round[8]; memcpy(round, top8, 8 * sizeof(Team));
    for (int r = 0, size = 8; r < 3; r++) {
        Team next[4]; int idx = 0;
        cout << rounds[r] << "s ready.\n";
        cout << "Type 'proceed' to play the " << rounds[r] << " or any other key to cancel: ";
        cin >> input;
        if (input != "proceed") {
            cout << "Tournament paused before " << rounds[r] << "s.\n";
            return;
        }
        for (int i = 0; i < size; i += 2)
            next[idx++] = simulateMatch(round[i], round[i + 1], rounds[r], 5, score);
        memcpy(round, next, idx * sizeof(Team));
        size = idx;
    }

    Team champion = round[0];
    showChampion(champion);

}

void addSpectator() {
    Spectator s;
    cin.ignore();
    cout << "Enter spectator name: ";
    getline(cin, s.name);
    cout << "Enter type (VIP / Streamer / General): ";
    getline(cin, s.type);

    if (s.type == "VIP") {
        vipQueue.push(s);
    } else if (s.type == "Streamer") {
        streamerQueue.push(s);
    } else if (s.type == "General") {
        generalQueue.push(s);
    } else {
        cout << "Invalid type. Please enter VIP, Streamer, or General.\n";
        return;
    }
    cout << "Added to " << s.type << " queue.\n";
}

void assignSeats() {
    static bool initialized = false;
    static int vipSeats, streamerSeats, generalSeats;

    if (!initialized) {
        cout << "Enter VIP seats: "; cin >> vipSeats;
        cout << "Enter General seats: "; cin >> generalSeats;
        initialized = true;
    }

    cout << "\n=== Assigning Seats ===\n";

    cout << "\nVIP Seats:\n";
    for (int i = 0; i < vipSeats && !vipQueue.empty(); i++) {
        Spectator s = vipQueue.front();
        cout << "  - " << s.name << endl;
        assignedVIPs[vipCount++] = s;
        vipQueue.pop();
    }

    cout << "\nGeneral Spectator Seats:\n";
    for (int i = 0; i < generalSeats && !generalQueue.empty(); i++) {
        Spectator s = generalQueue.front();
        cout << "  - " << s.name << endl;
        assignedGenerals[generalCount++] = s;
        generalQueue.pop();
    }
}

void setupViewingSlots() {
    int slots;
    cout << "\nEnter number of live stream slots: ";
    cin >> slots;
    cout << "\nLive stream slots have been allocated.\n";
    for (int i = 1; i <= slots && streamCount < 100; i++) {
        if (!streamerQueue.empty()) {
            assignedStreamers[streamCount++] = streamerQueue.front();
            streamerQueue.pop();
        }
    }
}

void viewSpectatorQueues() {
    cout << "\nQueue Sizes:\n";
    cout << "  VIP: " << vipQueue.size() << "\n";
    cout << "  Streamer: " << streamerQueue.size() << "\n";
    cout << "  General: " << generalQueue.size() << "\n";
}

void viewAssignedSpectators() {
    cout << "\n=== Assigned VIPs and General Spectators ===\n";
    cout << "VIP Spectators:\n";
    for (int i = 0; i < vipCount; i++) cout << "  - " << assignedVIPs[i].name << endl;
    cout << "General Spectators:\n";
    for (int i = 0; i < generalCount; i++) cout << "  - " << assignedGenerals[i].name << endl;
}

void viewLiveStreamers() {
    cout << "\n=== Assigned Streamers for Live Slots ===\n";
    for (int i = 0; i < streamCount; i++)
        cout << "Slot " << (i + 1) << ": " << assignedStreamers[i].name << endl;
}

void spectatorMenu() {
    int opt;
    do {
        cout << "\n=== Spectator Queue Management ===\n";
        cout << "1. Add Spectator\n";
        cout << "2. Assign Seats\n";
        cout << "3. Organize Live Stream Slots \n";
        cout << "4. View Queue Sizes\n";
        cout << "5. View Seated Spectators (VIP/General)\n";
        cout << "6. View Live Streamers\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> opt;
        switch (opt) {
            case 1: addSpectator(); break;
            case 2: assignSeats(); break;
            case 3: setupViewingSlots(); break;
            case 4: viewSpectatorQueues(); break;
            case 5: viewAssignedSpectators(); break;
            case 6: viewLiveStreamers(); break;
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (opt != 0);
}

void logMatch(string stage, string teamA, string teamB, string score, string winner) {
    MatchLog log;
    log.stage = stage;
    log.teamA = teamA;
    log.teamB = teamB;
    log.score = score;
    log.winner = winner;
    matchHistory[matchCount++] = log;
    teamWins[winner]++;

    ofstream outFile("match_logs.txt", ios::app);
    if (outFile.is_open()) {
        outFile << stage << "," << teamA << "," << teamB << "," << score << "," << winner << "\n";
        outFile.close();
    }
}

void viewMatchHistory() {
    cout << "\n=== Match History ===\n";
    for (int i = 0; i < matchCount; i++) {
        string stage = matchHistory[i].stage;

        if (stage == "Opening Match" || stage == "Winners Match" || 
            stage == "Elimination Match" || stage == "Decider Match") {
            if (i >= 0 && i < 5) stage = "Group A - " + stage;
            else if (i >= 5 && i < 10) stage = "Group B - " + stage;
            else if (i >= 10 && i < 15) stage = "Group C - " + stage;
            else if (i >= 15 && i < 20) stage = "Group D - " + stage;
        }

        cout << "[" << stage << "] ";
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

int main() {
    srand(time(0));
    readRegistrations();
    while (true) {
        char opt;
        cout << "\nLogin as (A)dmin or (T)eam or (Q)uit: ";
        cin >> opt;
        if (opt == 'A' || opt == 'a') {
            if (adminLogin()) adminMenu();
            else cout << "Login failed.\n";
        } else if (opt == 'T' || opt == 't') {
            processTeamCheckIn();
        } else if (opt == 'Q' || opt == 'q') {
            cout << "Goodbye and Have A Nice Day!\n"; break;
        } else {
            cout << "Invalid input.\n";
        }
    }
    return 0;
}
