#include <iostream>
#include <queue>
#include <string>
using namespace std;

struct Spectator {
    string name;
    string type;  // VIP / Streamer / General
    string stage; // Qualifiers / Group Stage / Quarterfinal / Semifinal / Grand Final
};

string stages[] = {"Qualifiers", "Group Stage", "Quarterfinal", "Semifinal", "Grand Final"};
const int MAX = 100;

queue<Spectator> vipQueues[5];
queue<Spectator> generalQueues[5];
queue<Spectator> streamerQueues[5];

Spectator seatedVIPs[5][MAX];     int vipCounts[5] = {0};
Spectator seatedGenerals[5][MAX]; int generalCounts[5] = {0};
Spectator seatedStreamers[5][MAX];int streamerCounts[5] = {0};

int vipSeats = 0, generalSeats = 0, streamSlots = 0;
bool seatsAssigned = false, streamsAssigned = false;

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

    cout << "The Seats Types (VIP / Streamer / General): ";
    getline(cin, s.type);

    cout << "Enter Stage (Qualifiers / Group Stage / Quarterfinal / Semifinal / Grand Final): ";
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

void showMenu() {
    int choice;
    do {
        cout << "\n=== Spectator Queue Management ===\n";
        cout << "1. Add Spectator\n";
        cout << "2. Assign Seats\n";
        cout << "3. Organize Live Stream Slots\n";
        cout << "4. View Queue Sizes\n";
        cout << "5. View Seated Spectators (VIP/General)\n";
        cout << "6. View Live Streamers\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addSpectator(); break;
            case 2: assignSeats(); break;
            case 3: setupViewingSlots(); break;
            case 4: viewSpectatorQueues(); break;
            case 5: viewAssignedSpectators(); break;
            case 6: viewLiveStreamers(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n"; break;
        }
    } while (choice != 0);
}

int main() {
    showMenu();
    return 0;
}
