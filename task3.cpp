#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

struct Spectator {
    string name;
    string type;  // VIP / General / Streamer
    string stage; // Tournament stage
    time_t registrationTime; // Time when spectator was added
    string contactInfo; // For notifications
};

string stages[] = {"Qualifiers", "Group Stage", "Quarterfinal", "Semifinal", "Grand Final"};
const int MAX_SEATS = 1000; // Increased maximum capacity

// Main queues for each stage
queue<Spectator> vipQueues[5];
queue<Spectator> generalQueues[5];
queue<Spectator> streamerQueues[5];

// Waitlists for overflow
queue<Spectator> waitlistedVIPs[5];
queue<Spectator> waitlistedGenerals[5];
queue<Spectator> waitlistedStreamers[5];

// Seated spectators
vector<Spectator> seatedVIPs[5];
vector<Spectator> seatedGenerals[5];
vector<Spectator> seatedStreamers[5];

// Capacity settings
int vipSeats = 50, generalSeats = 200, streamSlots = 20;
bool seatsAssigned = false, streamsAssigned = false;

// File names for persistence
const string DATA_FILE = "spectator_data.txt";
const string CONFIG_FILE = "seat_config.txt";

// Function prototypes
int getStageIndex(string stageName);
void loadConfig();
void saveConfig();
void addSpectator();
void assignSeats();
void setupViewingSlots();
void offerUpgrades();
void expandSeating();
void checkWaitlists();
void viewSpectatorQueues();
void viewAssignedSpectators();
void viewLiveStreamers();
void viewWaitlists();
void viewSeatAvailability();
void saveData();
void loadData();
void sendNotification(const Spectator& s, const string& message);
void showMenu();

int main() {
    loadConfig();
    loadData();
    showMenu();
    saveData();
    saveConfig();
    return 0;
}

int getStageIndex(string stageName) {
    for (int i = 0; i < 5; i++)
        if (stages[i] == stageName)
            return i;
    return -1;
}

void loadConfig() {
    ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        file >> vipSeats >> generalSeats >> streamSlots;
        file.close();
    }
}

void saveConfig() {
    ofstream file(CONFIG_FILE);
    if (file.is_open()) {
        file << vipSeats << " " << generalSeats << " " << streamSlots;
        file.close();
    }
}

void addSpectator() {
    Spectator s;
    cin.ignore();
    
    cout << "Enter spectator name: ";
    getline(cin, s.name);
    
    cout << "Seat Type (VIP/General/Streamer): ";
    getline(cin, s.type);
    
    cout << "Stage (Qualifiers/Group Stage/Quarterfinal/Semifinal/Grand Final): ";
    getline(cin, s.stage);
    
    cout << "Contact email/phone: ";
    getline(cin, s.contactInfo);
    
    s.registrationTime = time(nullptr);
    
    int index = getStageIndex(s.stage);
    if (index == -1) {
        cout << "Invalid stage.\n";
        return;
    }

    if (s.type == "VIP") {
        vipQueues[index].push(s);
    } else if (s.type == "General") {
        generalQueues[index].push(s);
    } else if (s.type == "Streamer") {
        streamerQueues[index].push(s);
    } else {
        cout << "Invalid type.\n";
        return;
    }

    cout << "Added to " << s.type << " queue for " << s.stage << ".\n";
    saveData();
}

void assignSeats() {
    if (!seatsAssigned) {
        cout << "Using default capacities - VIP: " << vipSeats 
             << ", General: " << generalSeats << endl;
        seatsAssigned = true;
    }

    for (int i = 0; i < 5; i++) {
        cout << "\n=== Processing " << stages[i] << " ===\n";
        
        // Assign VIP seats
        while (!vipQueues[i].empty()) {
            if (seatedVIPs[i].size() < vipSeats) {
                Spectator s = vipQueues[i].front();
                vipQueues[i].pop();
                seatedVIPs[i].push_back(s);
                cout << "VIP: " << s.name << " (Seated)\n";
                sendNotification(s, "You've been seated for " + stages[i] + " as VIP");
            } else {
                waitlistedVIPs[i].push(vipQueues[i].front());
                cout << "VIP: " << vipQueues[i].front().name << " (Waitlisted)\n";
                sendNotification(vipQueues[i].front(), 
                    "You've been waitlisted for " + stages[i] + " VIP seating");
                vipQueues[i].pop();
            }
        }

        // Assign General seats
        while (!generalQueues[i].empty()) {
            if (seatedGenerals[i].size() < generalSeats) {
                Spectator s = generalQueues[i].front();
                generalQueues[i].pop();
                seatedGenerals[i].push_back(s);
                cout << "General: " << s.name << " (Seated)\n";
                sendNotification(s, "You've been seated for " + stages[i] + " as General");
            } else {
                waitlistedGenerals[i].push(generalQueues[i].front());
                cout << "General: " << generalQueues[i].front().name << " (Waitlisted)\n";
                sendNotification(generalQueues[i].front(), 
                    "You've been waitlisted for " + stages[i] + " General seating");
                generalQueues[i].pop();
            }
        }
    }
    saveData();
}

void setupViewingSlots() {
    if (!streamsAssigned) {
        cout << "Using default streamer slots: " << streamSlots << endl;
        streamsAssigned = true;
    }

    for (int i = 0; i < 5; i++) {
        cout << "\n=== Assigning Streamers for " << stages[i] << " ===\n";
        while (!streamerQueues[i].empty()) {
            if (seatedStreamers[i].size() < streamSlots) {
                Spectator s = streamerQueues[i].front();
                streamerQueues[i].pop();
                seatedStreamers[i].push_back(s);
                cout << "Streamer: " << s.name << " (Assigned)\n";
                sendNotification(s, "You've been assigned a streaming slot for " + stages[i]);
            } else {
                waitlistedStreamers[i].push(streamerQueues[i].front());
                cout << "Streamer: " << streamerQueues[i].front().name << " (Waitlisted)\n";
                sendNotification(streamerQueues[i].front(), 
                    "You've been waitlisted for " + stages[i] + " streaming slot");
                streamerQueues[i].pop();
            }
        }
    }
    saveData();
}

void offerUpgrades() {
    for (int i = 0; i < 5; i++) {
        // Offer VIP upgrades to General spectators
        while (!waitlistedGenerals[i].empty() && seatedVIPs[i].size() < vipSeats) {
            Spectator s = waitlistedGenerals[i].front();
            waitlistedGenerals[i].pop();
            
            cout << "Offer VIP upgrade to " << s.name << "? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();
            
            if (tolower(choice) == 'y') {
                seatedVIPs[i].push_back(s);
                cout << s.name << " upgraded to VIP.\n";
                sendNotification(s, "You've been upgraded to VIP for " + stages[i]);
            } else {
                seatedGenerals[i].push_back(s);
                cout << s.name << " remains as General.\n";
            }
        }
    }
    saveData();
}

void expandSeating() {
    cout << "Current capacities:\n";
    cout << "VIP: " << vipSeats << "\nGeneral: " << generalSeats << "\nStreamers: " << streamSlots << endl;
    
    cout << "Enter additional VIP seats: ";
    int addVIP;
    cin >> addVIP;
    
    cout << "Enter additional General seats: ";
    int addGeneral;
    cin >> addGeneral;
    
    cout << "Enter additional Streamer slots: ";
    int addStreamer;
    cin >> addStreamer;
    
    vipSeats += addVIP;
    generalSeats += addGeneral;
    streamSlots += addStreamer;
    
    cout << "New capacities:\n";
    cout << "VIP: " << vipSeats << "\nGeneral: " << generalSeats << "\nStreamers: " << streamSlots << endl;
    
    saveConfig();
    checkWaitlists(); // Automatically check waitlists after expansion
}

void checkWaitlists() {
    for (int i = 0; i < 5; i++) {
        // Process VIP waitlist
        while (!waitlistedVIPs[i].empty() && seatedVIPs[i].size() < vipSeats) {
            Spectator s = waitlistedVIPs[i].front();
            waitlistedVIPs[i].pop();
            seatedVIPs[i].push_back(s);
            cout << "NOTICE: " << s.name << " seated from VIP waitlist.\n";
            sendNotification(s, "A VIP seat has become available for " + stages[i]);
        }
        
        // Process General waitlist
        while (!waitlistedGenerals[i].empty() && seatedGenerals[i].size() < generalSeats) {
            Spectator s = waitlistedGenerals[i].front();
            waitlistedGenerals[i].pop();
            seatedGenerals[i].push_back(s);
            cout << "NOTICE: " << s.name << " seated from General waitlist.\n";
            sendNotification(s, "A General seat has become available for " + stages[i]);
        }
        
        // Process Streamer waitlist
        while (!waitlistedStreamers[i].empty() && seatedStreamers[i].size() < streamSlots) {
            Spectator s = waitlistedStreamers[i].front();
            waitlistedStreamers[i].pop();
            seatedStreamers[i].push_back(s);
            cout << "NOTICE: " << s.name << " assigned from Streamer waitlist.\n";
            sendNotification(s, "A streaming slot has become available for " + stages[i]);
        }
    }
    saveData();
}

void viewSpectatorQueues() {
    cout << "\n=== Current Queue Sizes ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "  VIP Queue: " << vipQueues[i].size() << "\n";
        cout << "  General Queue: " << generalQueues[i].size() << "\n";
        cout << "  Streamer Queue: " << streamerQueues[i].size() << "\n";
    }
}

void viewAssignedSpectators() {
    cout << "\n=== Seated Spectators ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        
        cout << "VIP (" << seatedVIPs[i].size() << "/" << vipSeats << "):\n";
        for (const auto& s : seatedVIPs[i])
            cout << "  - " << s.name << " (" << s.contactInfo << ")\n";
            
        cout << "General (" << seatedGenerals[i].size() << "/" << generalSeats << "):\n";
        for (const auto& s : seatedGenerals[i])
            cout << "  - " << s.name << " (" << s.contactInfo << ")\n";
    }
}

void viewLiveStreamers() {
    cout << "\n=== Assigned Streamers ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "Streamers (" << seatedStreamers[i].size() << "/" << streamSlots << "):\n";
        for (const auto& s : seatedStreamers[i])
            cout << "  - " << s.name << " (" << s.contactInfo << ")\n";
    }
}

void viewWaitlists() {
    cout << "\n=== Waitlist Counts ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "  VIP Waitlist: " << waitlistedVIPs[i].size() << "\n";
        cout << "  General Waitlist: " << waitlistedGenerals[i].size() << "\n";
        cout << "  Streamer Waitlist: " << waitlistedStreamers[i].size() << "\n";
    }
}

void viewSeatAvailability() {
    cout << "\n=== Available Seats ===\n";
    for (int i = 0; i < 5; i++) {
        cout << "\n[" << stages[i] << "]\n";
        cout << "  VIP: " << (vipSeats - seatedVIPs[i].size()) << "/" << vipSeats << "\n";
        cout << "  General: " << (generalSeats - seatedGenerals[i].size()) << "/" << generalSeats << "\n";
        cout << "  Streamer: " << (streamSlots - seatedStreamers[i].size()) << "/" << streamSlots << "\n";
    }
}

void saveData() {
    ofstream file(DATA_FILE);
    if (file.is_open()) {
        // Save capacities
        file << vipSeats << " " << generalSeats << " " << streamSlots << "\n";
        
        // Save spectators in queues
        for (int i = 0; i < 5; i++) {
            queue<Spectator> temp;
            
            // VIP queue
            file << vipQueues[i].size() << "\n";
            temp = vipQueues[i];
            while (!temp.empty()) {
                file << temp.front().name << "\n" << temp.front().type << "\n" 
                     << temp.front().stage << "\n" << temp.front().contactInfo << "\n"
                     << temp.front().registrationTime << "\n";
                temp.pop();
            }
            
            // Repeat for other queues and waitlists...
        }
        
        file.close();
    }
}

void loadData() {
    ifstream file(DATA_FILE);
    if (file.is_open()) {
        // Load capacities
        file >> vipSeats >> generalSeats >> streamSlots;
        
        // Load spectators in queues
        for (int i = 0; i < 5; i++) {
            int count;
            string line;
            
            // VIP queue
            file >> count;
            file.ignore();
            for (int j = 0; j < count; j++) {
                Spectator s;
                getline(file, s.name);
                getline(file, s.type);
                getline(file, s.stage);
                getline(file, s.contactInfo);
                file >> s.registrationTime;
                file.ignore();
                vipQueues[i].push(s);
            }
            
            // Repeat for other queues and waitlists...
        }
        
        file.close();
    }
}

void sendNotification(const Spectator& s, const string& message) {
    // In a real implementation, this would send email/SMS
    cout << "\n=== Notification to " << s.name << " ===\n";
    cout << "Contact: " << s.contactInfo << "\n";
    cout << "Message: " << message << "\n\n";
}

void showMenu() {
    int choice;
    do {
        cout << "\n=== Enhanced Spectator Management System ===\n";
        cout << "1. Add Spectator\n";
        cout << "2. Assign Seats\n";
        cout << "3. Organize Live Stream Slots\n";
        cout << "4. View Queue Sizes\n";
        cout << "5. View Seated Spectators\n";
        cout << "6. View Live Streamers\n";
        cout << "7. View Waitlists\n";
        cout << "8. View Seat Availability\n";
        cout << "9. Offer Upgrades\n";
        cout << "10. Expand Seating Capacity\n";
        cout << "11. Check Waitlists for Availability\n";
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
            case 7: viewWaitlists(); break;
            case 8: viewSeatAvailability(); break;
            case 9: offerUpgrades(); break;
            case 10: expandSeating(); break;
            case 11: checkWaitlists(); break;
            case 0: cout << "Exiting system...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}