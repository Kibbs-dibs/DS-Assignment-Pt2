#include <iostream>
#include <queue>
using namespace std;

struct Spectator {
    string name;
    string type; // VIP, Streamer, General
};

queue<Spectator> vipQueue;
queue<Spectator> streamerQueue;
queue<Spectator> generalQueue;

void addSpectator() {
    Spectator s;
    cout << "Enter spectator name: ";
    cin.ignore(); getline(cin, s.name);
    cout << "Enter type (VIP / Streamer / General): ";
    getline(cin, s.type);
    if (s.type == "VIP") vipQueue.push(s);
    else if (s.type == "Streamer") streamerQueue.push(s);
    else generalQueue.push(s);
    cout << "Added to " << s.type << " queue.\n";
}

void assignSeats(int vipSeats, int streamerSeats, int generalSeats) {
    cout << "\n=== Assigning Seats ===\n";

    cout << "\nVIP Seats:\n";
    for (int i = 0; i < vipSeats && !vipQueue.empty(); i++) {
        cout << "  - " << vipQueue.front().name << endl;
        vipQueue.pop();
    }

    cout << "\nStreamer Seats:\n";
    for (int i = 0; i < streamerSeats && !streamerQueue.empty(); i++) {
        cout << "  - " << streamerQueue.front().name << endl;
        streamerQueue.pop();
    }

    cout << "\nGeneral Spectator Seats:\n";
    for (int i = 0; i < generalSeats && !generalQueue.empty(); i++) {
        cout << "  - " << generalQueue.front().name << endl;
        generalQueue.pop();
    }
}

void setupViewingSlots() {
    int slots;
    cout << "\nEnter number of live stream slots: ";
    cin >> slots;
    cout << "\nAssigning Live Stream Slots (Streamers Only)\n";
    for (int i = 1; i <= slots && !streamerQueue.empty(); i++) {
        cout << "Slot " << i << ": " << streamerQueue.front().name << " (Streamer)\n";
        streamerQueue.pop();
    }
}

void viewSpectatorQueues() {
    cout << "\nQueue Sizes:\n";
    cout << "  VIP: " << vipQueue.size() << "\n";
    cout << "  Streamer: " << streamerQueue.size() << "\n";
    cout << "  General: " << generalQueue.size() << "\n";
}

void spectatorMenu() {
    int opt;
    do {
        cout << "\n=== Spectator Queue Management ===\n";
        cout << "1. Add Spectator\n";
        cout << "2. Assign Seats\n";
        cout << "3. View Queue Sizes\n";
        cout << "4. Organize Live Stream Slots\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        cin >> opt;
        switch (opt) {
            case 1: addSpectator(); break;
            case 2: {
                int v, s, g;
                cout << "Enter VIP seats: "; cin >> v;
                cout << "Enter Streamer seats: "; cin >> s;
                cout << "Enter General seats: "; cin >> g;
                assignSeats(v, s, g);
                break;
            }
            case 3: viewSpectatorQueues(); break;
            case 4: setupViewingSlots(); break;
            case 0: break;
            default: cout << "Invalid option.\n";
        }
    } while (opt != 0);
}
