#include <iostream>
#include <queue>
#include <string>
#include <algorithm>
using namespace std;

struct Spectator {
    string name;
    string type;  // VIP, Streamer, General
    string stage; // Qualifiers, Group Stage, Double Elimination Playoffs
};

const int STAGE_COUNT = 3;
string stages[STAGE_COUNT] = {"Qualifiers", "Group Stage", "Double Elimination Playoffs"};

queue<Spectator> vipQueues[STAGE_COUNT];
queue<Spectator> streamerQueues[STAGE_COUNT];
queue<Spectator> generalQueues[STAGE_COUNT];

Spectator seatedVIPs[STAGE_COUNT][100]; int vipCounts[STAGE_COUNT] = {0};
Spectator seatedGenerals[STAGE_COUNT][100]; int generalCounts[STAGE_COUNT] = {0};
Spectator seatedStreamers[STAGE_COUNT][100]; int streamerCounts[STAGE_COUNT] = {0};

int vipSeats = 0, generalSeats = 0, streamerSeats = 0;
bool seatsSet = false;

int getStageIndex(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s == "qualifiers") return 0;
    if (s == "group stage") return 1;
    if (s == "double elimination playoffs") return 2;
    return -1;
}

void setSeatLimits() {
    cout << "\n=== Set Seat Limits ===\n";
    cout << "Enter VIP seats: "; cin >> vipSeats;
    cout << "Enter Streamer seats: "; cin >> streamerSeats;
    cout << "Enter General seats: "; cin >> generalSeats;
    seatsSet = true;
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

    if (s.type == "VIP") {
        if (vipCounts[index] >= vipSeats) {
            cout << "Cannot add. VIP seats are already full for " << stages[index] << ".\n";
            return;
        }
        vipQueues[index].push(s);
    } else if (s.type == "General") {
        if (generalCounts[index] >= generalSeats) {
            cout << "Cannot add. General seats are already full for " << stages[index] << ".\n";
            return;
        }
        generalQueues[index].push(s);
    } else if (s.type == "Streamer") {
        if (streamerCounts[index] >= streamerSeats) {
            cout << "Cannot add. Streamer seats are already full for " << stages[index] << ".\n";
            return;
        }
        streamerQueues[index].push(s);
    } else {
        cout << "Invalid type.\n";
        return;
    }
    cout << "Added to " << s.type << " queue for " << s.stage << ".\n";
}

void assignSeats() {
    if (!seatsSet) setSeatLimits();

    for (int i = 0; i < STAGE_COUNT; i++) {
        cout << "\nAssigning seats for " << stages[i] << "...\n";

        int vipAssigned = 0;
        while (vipAssigned < vipSeats && !vipQueues[i].empty()) {
            Spectator s = vipQueues[i].front(); vipQueues[i].pop();
            seatedVIPs[i][vipCounts[i]++] = s;
            cout << "VIP: " << s.name << endl;
            vipAssigned++;
        }
        if (!vipQueues[i].empty()) {
            cout << "VIP seats full for " << stages[i] << ". Consider increasing seat limits.\n";
        }

        int generalAssigned = 0;
        while (generalAssigned < generalSeats && !generalQueues[i].empty()) {
            Spectator s = generalQueues[i].front(); generalQueues[i].pop();
            seatedGenerals[i][generalCounts[i]++] = s;
            cout << "General: " << s.name << endl;
            generalAssigned++;
        }
        if (!generalQueues[i].empty()) {
            cout << "General seats full for " << stages[i] << ". Consider increasing seat limits.\n";
        }
    }
}

void setupViewingSlots() {
    if (!seatsSet) setSeatLimits();

    for (int i = 0; i < STAGE_COUNT; i++) {
        cout << "\nAssigning streamers for " << stages[i] << "...\n";
        int streamAssigned = 0;
        while (streamAssigned < streamerSeats && !streamerQueues[i].empty()) {
            Spectator s = streamerQueues[i].front(); streamerQueues[i].pop();
            seatedStreamers[i][streamerCounts[i]++] = s;
            cout << s.name << endl;
            streamAssigned++;
        }
        if (!streamerQueues[i].empty()) {
            cout << "Streamer slots full for " << stages[i] << ". Consider increasing seat limits.\n";
        }
    }
}

void viewSpectatorQueues() {
    cout << "\n=== Queue Sizes by Stage ===\n";
    for (int i = 0; i < STAGE_COUNT; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "  VIP Queue: " << vipQueues[i].size() << "\n";
        cout << "  General Queue: " << generalQueues[i].size() << "\n";
        cout << "  Streamer Queue: " << streamerQueues[i].size() << "\n";
    }
}

void viewAssignedSpectators() {
    cout << "\n=== Seated VIP & General Spectators ===\n";
    for (int i = 0; i < STAGE_COUNT; i++) {
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
    for (int i = 0; i < STAGE_COUNT; i++) {
        cout << "\n[" << stages[i] << "]\n";
        for (int j = 0; j < streamerCounts[i]; j++)
            cout << seatedStreamers[i][j].name << endl;
    }
}

void spectatorMenu() {
    int opt;
    do {
        cout << "\n=== Spectator Queue Management ===\n";
        cout << "1. Add Spectator\n";
        cout << "2. Assign Seats\n";
        cout << "3. View Queue Sizes\n";
        cout << "4. Organize Live Stream Slots\n";
        cout << "5. View Seated Spectators (VIP/General)\n";
        cout << "6. View Assigned Live Streamers\n";
        cout << "7. Change Seat Limits\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> opt;
        switch (opt) {
            case 1: addSpectator(); break;
            case 2: assignSeats(); break;
            case 3: viewSpectatorQueues(); break;
            case 4: setupViewingSlots(); break;
            case 5: viewAssignedSpectators(); break;
            case 6: viewLiveStreamers(); break;
            case 7: setSeatLimits(); break;
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (opt != 0);
}
