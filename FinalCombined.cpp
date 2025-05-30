#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <queue>
#include <limits>
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
    string stage; 
};
string stages[] = {"Qualifiers", "Group Stage", "Quarterfinal", "Semifinal", "Grand Final"};
const int MAX = 100;


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

Team registrationQueue[64], checkedIn[32];
int regCount = 0, checkedCount = 0;

queue<Spectator> vipQueues[5];
queue<Spectator> generalQueues[5];
queue<Spectator> streamerQueues[5];
Spectator seatedVIPs[5][MAX];     int vipCounts[5] = {0};
Spectator seatedGenerals[5][MAX]; int generalCounts[5] = {0};
Spectator seatedStreamers[5][MAX];int streamerCounts[5] = {0};
int vipSeats = 0, generalSeats = 0, streamSlots = 0;
bool seatsAssigned = false, streamsAssigned = false;

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
void startTournament(Team checkedIn[32]);

int calculateTeamStrength(Team &t) {
    int sum = 0;
    for (int i = 0; i < 5; i++) sum += rankStrength(t.players[i].rank);
    return sum;
}

bool isTeamAlreadyCheckedIn(const char* teamName) {
    for (int i = 0; i < checkedCount; i++) {
        if (strcmp(checkedIn[i].name, teamName) == 0)
            return true;
    }
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

void viewAllUnregisteredTeams() {
    cout << "\n=== Unchecked-In Registered Teams ===\n";
    for (int i = 0; i < regCount; i++) {
        if (!isTeamAlreadyCheckedIn(registrationQueue[i].name)) {
            cout << i + 1 << ". " << registrationQueue[i].name << endl;
        }
    }
}

void adminSingleCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full (32 teams).\n";
        return;
    }

    viewAllUnregisteredTeams();

    int choice;
    cout << "\nSelect team number to check in: ";
    cin >> choice;
    cin.ignore();

    if (choice < 1 || choice > regCount || isTeamAlreadyCheckedIn(registrationQueue[choice - 1].name)) {
        cout << "Invalid selection or team already checked in.\n";
        return;
    }

    string priority;
    cout << "Enter registration type (earlybird/wildcard): ";
    cin >> priority;
    if (priority == "earlybird")
        strcpy(registrationQueue[choice - 1].regType, "earlybird");
    else if (priority == "wildcard")
        strcpy(registrationQueue[choice - 1].regType, "wildcard");
    else {
        cout << "Invalid type. Defaulting to general.\n";
        strcpy(registrationQueue[choice - 1].regType, "general");
    }

    checkedIn[checkedCount++] = registrationQueue[choice - 1];
    cout << registrationQueue[choice - 1].name << " has been checked in.\n";
}

void withdrawAndReplace() {
    cout << "\n--- Withdraw and Replace ---\n";
    for (int i = 0; i < checkedCount; i++) {
        cout << i + 1 << ". " << checkedIn[i].name << endl;
    }

    int withdrawChoice;
    cout << "Enter team number to withdraw: ";
    cin >> withdrawChoice;
    cin.ignore();

    if (withdrawChoice < 1 || withdrawChoice > checkedCount) {
        cout << "Invalid choice.\n";
        return;
    }

    viewAllUnregisteredTeams();

    int replaceChoice;
    cout << "Enter replacement team number: ";
    cin >> replaceChoice;
    cin.ignore();

    if (replaceChoice < 1 || replaceChoice > regCount || isTeamAlreadyCheckedIn(registrationQueue[replaceChoice - 1].name)) {
        cout << "Invalid or already checked-in team.\n";
        return;
    }

    string priority;
    cout << "Enter registration type for replacement (earlybird/wildcard): ";
    cin >> priority;
    if (priority == "earlybird")
        strcpy(registrationQueue[replaceChoice - 1].regType, "earlybird");
    else if (priority == "wildcard")
        strcpy(registrationQueue[replaceChoice - 1].regType, "wildcard");
    else {
        cout << "Invalid type. Defaulting to general.\n";
        strcpy(registrationQueue[replaceChoice - 1].regType, "general");
    }

    checkedIn[withdrawChoice - 1] = registrationQueue[replaceChoice - 1];
    cout << "Replaced with " << registrationQueue[replaceChoice - 1].name << endl;
}

void processTeamCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full.\n";
        return;
    }

    viewAllUnregisteredTeams();

    int choice;
    cout << "\nSelect your team number: ";
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore();

    if (choice < 1 || choice > regCount || isTeamAlreadyCheckedIn(registrationQueue[choice - 1].name)) {
        cout << "Invalid selection or already checked in.\n";
        return;
    }

    string priority;
    cout << "Enter registration type (earlybird/wildcard): ";
    cin >> priority;
    if (priority == "earlybird")
        strcpy(registrationQueue[choice - 1].regType, "earlybird");
    else if (priority == "wildcard")
        strcpy(registrationQueue[choice - 1].regType, "wildcard");
    else {
        cout << "Invalid type. Defaulting to general.\n";
        strcpy(registrationQueue[choice - 1].regType, "general");
    }

    checkedIn[checkedCount++] = registrationQueue[choice - 1];
    cout << registrationQueue[choice - 1].name << " checked in successfully.\n";

    if (checkedCount == 32) {
        cout << "\nAll 32 teams have been registered.\n";
        cout << "Waiting for Admin to start the tournament.\n";
    }
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
        while (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number: ";
        }
        switch (choice) {
            case 1: adminSingleCheckIn(); break;
            case 2: withdrawAndReplace(); break;
            case 3: viewCheckedInTeams(); break;
            case 4: spectatorMenu(); break;
            case 5: startTournament(checkedIn); break;
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

int getStageIndex(string stageName) {
    for (int i = 0; i < 5; i++)
        if (stages[i] == stageName)
            return i;
    return -1;
}

void addSpectator() {
    Spectator s;
    cin.ignore();
    cout << "Enter spectator name: ";
    getline(cin, s.name);
    cout << "Buy Seats Types (VIP / Streamer / General): ";
    getline(cin, s.type);
    cout << "For Which Match Day (Qualifiers / Group Stage / Double Elimination Playoffs): ";
    getline(cin, s.stage);
    int index = getStageIndex(s.stage);
    if (index == -1) {
        cout << "Invalid stage.\n";
        return;
    }
    if (s.type == "VIP")
        vipQueues[index].push(s);
    else if (s.type == "General")
        generalQueues[index].push(s);
    else if (s.type == "Streamer")
        streamerQueues[index].push(s);
    else {
        cout << "Invalid type.\n";
        return;
    }
    cout << "Added to " << s.type << " queue for " << s.stage << ".\n";
}

void assignSeats() {
    if (!seatsAssigned) {
        cout << "Enter number of VIP seats per stage: ";
        cin >> vipSeats;
        cout << "Enter number of General seats per stage: ";
        cin >> generalSeats;
        seatsAssigned = true;
    }
    for (int i = 0; i < 5; i++) {
        cout << "\nAssigning seats for " << stages[i] << "...\n";
        // VIP
        for (int j = 0; j < vipSeats && !vipQueues[i].empty(); j++) {
            Spectator s = vipQueues[i].front(); vipQueues[i].pop();
            seatedVIPs[i][vipCounts[i]++] = s;
            cout << "VIP: " << s.name << endl;
        }
        // General
        for (int j = 0; j < generalSeats && !generalQueues[i].empty(); j++) {
            Spectator s = generalQueues[i].front(); generalQueues[i].pop();
            seatedGenerals[i][generalCounts[i]++] = s;
            cout << "General: " << s.name << endl;
        }
    }
}

void setupViewingSlots() {
    if (!streamsAssigned) {
        cout << "Enter number of streamers per stage: ";
        cin >> streamSlots;
        streamsAssigned = true;
    }
    for (int i = 0; i < 5; i++) {
        cout << "\nAssigning streamers for " << stages[i] << "...\n";
        for (int j = 0; j < streamSlots && !streamerQueues[i].empty(); j++) {
            Spectator s = streamerQueues[i].front(); streamerQueues[i].pop();
            seatedStreamers[i][streamerCounts[i]++] = s;
            cout << s.name << endl;
        }
    }
}

void viewSpectatorQueues() {
    cout << "\n=== Queue Sizes by Stage ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "  VIP Queue: " << vipQueues[i].size() << "\n";
        cout << "  General Queue: " << generalQueues[i].size() << "\n";
        cout << "  Streamer Queue: " << streamerQueues[i].size() << "\n";
    }
}

void viewAssignedSpectators() {
    cout << "\n=== Seated VIP & General Spectators ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "VIPs:\n";
        for (int j = 0; j < vipCounts[i]; j++)
            cout << "  - " << seatedVIPs[i][j].name << endl;
        cout << "Generals:\n";
        for (int j = 0; j < generalCounts[i]; j++)
            cout << "  - " << seatedGenerals[i][j].name << endl;
    }
}

void viewLiveStreamers() {
    cout << "\n=== Seated Streamers ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        for (int j = 0; j < streamerCounts[i]; j++) {
            cout << seatedStreamers[i][j].name << endl;
        }
    }
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

void startNewTournamentSession() {
    sessionMatchStartIndex = matchCount;
    teamWins.clear();
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
