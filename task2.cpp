#include <iostream>
#include <fstream>
#include <cstring>
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
    bool lastMinute = false;
};

Team registrationQueue[64], checkedIn[32];
int regCount = 0, checkedCount = 0;

bool adminLogin() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";
    cin >> password;
    return (username == "admin" && password == "1234");
}

bool isTeamAlreadyCheckedIn(const char* teamName) {
    for (int i = 0; i < checkedCount; i++) {
        if (strcmp(checkedIn[i].name, teamName) == 0)
            return true;
    }
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
    for (int i = 0; i < checkedCount; i++) {
        cout << i + 1 << ". " << checkedIn[i].name << " (" << checkedIn[i].regType << ")";
        if (checkedIn[i].lastMinute) {
            cout << " [last minute check-in]";
        }
        cout << "\n";
    }
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

    registrationQueue[choice - 1].lastMinute = true;
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
    cin >> choice;
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

void adminMenu() {
    int choice;
    do {
        cout << "\n=== Admin Menu ===\n";
        cout << "1. Last-Minute Check-In Team\n";
        cout << "2. Withdraw and replace a team\n";
        cout << "3. View checked-in teams\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: adminSingleCheckIn(); break;
            case 2: withdrawAndReplace(); break;
            case 3: viewCheckedInTeams(); break;
            case 0: cout << "Returning to login menu.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

int main() {
    readRegistrations();
    while (true) {
        char opt;
        cout << "\nLogin as (A)dmin or (T)eam or (E)xit: ";
        cin >> opt;
        if (opt == 'A' || opt == 'a') {
            if (adminLogin()) adminMenu();
            else cout << "Login failed.\n";
        } else if (opt == 'T' || opt == 't') {
            processTeamCheckIn();
        } else if (opt == 'E' || opt == 'e') {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid input.\n";
        }
    }
    return 0;
}
