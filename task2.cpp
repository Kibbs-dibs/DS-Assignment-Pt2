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
};

Team registrationQueue[64], checkedIn[32];
int regCount = 0, checkedCount = 0;

bool isTeamAlreadyCheckedIn(const char* teamName) {
    for (int i = 0; i < checkedCount; i++)
        if (strcmp(checkedIn[i].name, teamName) == 0)
            return true;
    return false;
}

bool adminLogin() {
    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";
    cin >> password;
    return (username == "admin" && password == "1234"); // you can change this
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

void processTeamCheckIn() {
    if (checkedCount >= 32) {
        cout << "Tournament is full.\n";
        return;
    }
    char name[50];
    cout << "Enter your team name: ";
    cin.ignore(); cin.getline(name, 50);
    for (int i = 0; i < regCount; i++) {
        if (strcmp(registrationQueue[i].name, name) == 0) {
            if (isTeamAlreadyCheckedIn(name)) {
                cout << "Already checked in.\n";
                return;
            }
            checkedIn[checkedCount++] = registrationQueue[i];
            cout << name << " checked in successfully.\n";
            viewCheckedInTeams();
            return;
        }
    }
    cout << "Team not found in registration list.\n";
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
        char option;
        cout << "\nLogin as (A)dmin or (T)eam or (Q)uit: ";
        cin >> option;

        if (option == 'A' || option == 'a') {
            if (adminLogin()) {
                adminMenu();
            } else {
                cout << "Login failed. Returning to main menu.\n";
            }
        } else if (option == 'T' || option == 't') {
            processTeamCheckIn();
        } else if (option == 'Q' || option == 'q') {
            cout << "Exiting program.\n";
            break;
        } else {
            cout << "Invalid input.\n";
        }
    }

    return 0;
}
