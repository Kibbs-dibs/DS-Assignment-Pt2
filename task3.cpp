#include <iostream>
#include <queue>
#include <string>
using namespace std;

struct Spectator {
    string name;
    string type;
};

queue<Spectator> vipQueue;
queue<Spectator> streamerQueue;
queue<Spectator> generalQueue;
Spectator assignedVIPs[100]; int vipCount = 0;
Spectator assignedGenerals[100]; int generalCount = 0;
Spectator assignedStreamers[100]; int streamCount = 0;

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
        cout << "\U0001F3A5 Slot " << (i + 1) << ": " << assignedStreamers[i].name << endl;
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
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (opt != 0);
}
