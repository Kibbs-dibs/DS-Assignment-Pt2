#include <iostream>
#include <cstring>
#include <limits>
using namespace std;

const int MAX_PLAYERS = 100;

struct Player {
    char name[50];
    bool isEarlyBird;
    bool isWildcard;
    bool isCheckedIn;
};

class PlayerQueue {
public:
    Player queue[MAX_PLAYERS];
    int front, rear;

    PlayerQueue() {
        front = 0;
        rear = -1;
    }

    bool isFull() {
        return rear == MAX_PLAYERS - 1;
    }

    bool isEmpty() {
        return front > rear;
    }

    void enqueue(Player p) {
        if (!isFull()) {
            rear++;
            queue[rear] = p;
            cout << p.name << " registered successfully.\n";
        } else {
            cout << "Queue is full.\n";
        }
    }

    void checkInPlayer(const char* playerName) {
        for (int i = front; i <= rear; i++) {
            if (strcmp(queue[i].name, playerName) == 0) {
                queue[i].isCheckedIn = true;
                cout << playerName << " checked in successfully.\n";
                return;
            }
        }
        cout << "Player not found.\n";
    }

    void withdrawAndReplace(const char* playerName) {
        for (int i = front; i <= rear; i++) {
            if (strcmp(queue[i].name, playerName) == 0) {
                cout << playerName << " has been withdrawn.\n";

                Player replacement;
                char early[10], wild[10];

                cout << "Enter replacement player name: ";
                cin.getline(replacement.name, 50);
                cout << "Is Early Bird? (yes/no): ";
                cin >> early;
                cout << "Is Wildcard Entry? (yes/no): ";
                cin >> wild;

                replacement.isEarlyBird = strcmp(early, "yes") == 0;
                replacement.isWildcard = strcmp(wild, "yes") == 0;
                replacement.isCheckedIn = false;

                queue[i] = replacement;
                cout << "Replacement " << replacement.name << " added successfully.\n";

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
        }
        cout << "Player not found.\n";
    }

    void displayQueue() {
        cout << "\n--- All Registered Players ---\n";
        if (isEmpty()) {
            cout << "No players in the queue.\n";
            return;
        }
        for (int i = front; i <= rear; i++) {
            cout << i + 1 << ". " << queue[i].name
                 << " | EarlyBird: " << (queue[i].isEarlyBird ? "Yes" : "No")
                 << " | Wildcard: " << (queue[i].isWildcard ? "Yes" : "No")
                 << " | CheckedIn: " << (queue[i].isCheckedIn ? "Yes" : "No")
                 << endl;
        }
    }

    void displayCheckedInPlayers() {
        cout << "\n--- Checked-In Players ---\n";
        bool any = false;
        for (int i = front; i <= rear; i++) {
            if (queue[i].isCheckedIn) {
                cout << queue[i].name << " | EarlyBird: " << (queue[i].isEarlyBird ? "Yes" : "No")
                     << " | Wildcard: " << (queue[i].isWildcard ? "Yes" : "No") << endl;
                any = true;
            }
        }
        if (!any) cout << "No players have checked in yet.\n";
    }

    int countUnchecked() {
        int count = 0;
        for (int i = front; i <= rear; i++) {
            if (!queue[i].isCheckedIn)
                count++;
        }
        return count;
    }

    void viewPlayerProfile(const char* playerName) {
        for (int i = front; i <= rear; i++) {
            if (strcmp(queue[i].name, playerName) == 0) {
                cout << "\n--- Player Profile ---\n";
                cout << "Name      : " << queue[i].name << endl;
                cout << "EarlyBird : " << (queue[i].isEarlyBird ? "Yes" : "No") << endl;
                cout << "Wildcard  : " << (queue[i].isWildcard ? "Yes" : "No") << endl;
                cout << "CheckedIn : " << (queue[i].isCheckedIn ? "Yes" : "No") << endl;
                return;
            }
        }
        cout << "Player not found.\n";
    }
};

Player createPlayer() {
    Player p;
    char early[10], wild[10];

    cout << "Enter player name: ";
    cin.getline(p.name, 50);
    cout << "Is Early Bird? (yes/no): ";
    cin >> early;
    cout << "Is Wildcard Entry? (yes/no): ";
    cin >> wild;

    p.isEarlyBird = strcmp(early, "yes") == 0;
    p.isWildcard = strcmp(wild, "yes") == 0;
    p.isCheckedIn = false;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return p;
}

void adminPanel(PlayerQueue &pq) {
    int choice;
    char tempName[50];

    do {
        cout << "\n--- ADMIN PANEL ---\n";
        cout << "1. Register Player\n";
        cout << "2. Last-Minute Check-In\n";
        cout << "3. Withdraw and Replace Player\n";
        cout << "4. View All Registered Players\n";
        cout << "5. Start Tournament\n";
        cout << "6. Back to Main Menu\n";
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                pq.enqueue(createPlayer());
                break;
            case 2:
                cout << "Enter player name to check-in: ";
                cin.getline(tempName, 50);
                pq.checkInPlayer(tempName);
                break;
            case 3:
                cout << "Enter player name to withdraw and replace: ";
                cin.getline(tempName, 50);
                pq.withdrawAndReplace(tempName);
                break;
            case 4:
                pq.displayQueue();
                break;
            case 5: {
                int unchecked = pq.countUnchecked();
                if (unchecked > 0) {
                    cout << "\nWarning: " << unchecked << " player(s) not checked in.\n";
                    cout << "Start tournament anyway? (yes/no): ";
                    char confirm[10];
                    cin >> confirm;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (strcmp(confirm, "yes") != 0) break;
                }
                cout << "\nTournament is starting with the following players:\n";
                pq.displayCheckedInPlayers();
                break;
            }
            case 6:
                return;
            default:
                cout << "Invalid option. Try again.\n";
        }
    } while (choice != 6);
}

void playerPanel(PlayerQueue &pq) {
    int choice;
    char name[50];

    cout << "\n--- PLAYER PANEL ---\n";
    cout << "Enter your name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(name, 50);

    do {
        cout << "\n1. View My Profile\n";
        cout << "2. Check-In\n";
        cout << "3. Back to Main Menu\n";
        cout << "Choose an option: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                pq.viewPlayerProfile(name);
                break;
            case 2:
                pq.checkInPlayer(name);
                break;
            case 3:
                return;
            default:
                cout << "Invalid option. Try again.\n";
        }
    } while (choice != 3);
}

bool adminLogin() {
    const char USERNAME[] = "admin";
    const char PASSWORD[] = "pass123";

    char inputUser[50], inputPass[50];

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter Admin Username: ";
    cin.getline(inputUser, 50);
    cout << "Enter Admin Password: ";
    cin.getline(inputPass, 50);

    if (strcmp(inputUser, USERNAME) == 0 && strcmp(inputPass, PASSWORD) == 0) {
        cout << "Login successful.\n";
        return true;
    } else {
        cout << "Invalid login. Returning to main menu.\n";
        return false;
    }
}

int main() {
    PlayerQueue pq;
    int userType;

    do {
        cout << "\n=== APU ESPORT SYSTEM ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Player Access\n";
        cout << "3. Exit\n";
        cout << "Choose user type: ";
        cin >> userType;

        switch (userType) {
            case 1:
                if (adminLogin()) adminPanel(pq);
                break;
            case 2:
                playerPanel(pq);
                break;
            case 3:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid option. Try again.\n";
        }
    } while (userType != 3);

    return 0;
}
