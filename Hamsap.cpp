#include <iostream>
#include <cstring>
#include <limits>
using namespace std;

const int MAX_PLAYERS = 100;
const int MAX_SPECTATORS = 100;
const int MAX_MATCHES = 100;
const int MAX_RESULTS = 100;

void pauseScreen() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void clearScreen() {
    cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the screen
}

struct Player {
    char name[50];
    bool isEarlyBird;
    bool isWildcard;
    bool isCheckedIn;
};

struct Spectator {
    char name[50];
};

struct Match {
    char player1[50];
    char player2[50];
    char winner[50];
};

class MatchStack {
    Match matches[MAX_MATCHES];
    int top;

public:
    MatchStack() { top = -1; }

    bool isFull() { return top == MAX_MATCHES - 1; }
    bool isEmpty() { return top == -1; }

    void pushMatch(const char* p1, const char* p2, const char* winner) {
        if (isFull()) {
            cout << "Match stack full.\n";
            return;
        }
        top++;
        strcpy(matches[top].player1, p1);
        strcpy(matches[top].player2, p2);
        strcpy(matches[top].winner, winner);
        cout << "Match result pushed to stack.\n";
    }

    void popMatch() {
        if (isEmpty()) {
            cout << "No match to pop.\n";
            return;
        }
        cout << "Popped match: " << matches[top].player1 << " vs " << matches[top].player2
             << ", Winner: " << matches[top].winner << endl;
        top--;
    }

    void viewTopMatch() {
        if (isEmpty()) {
            cout << "No recent match.\n";
            return;
        }
        cout << "Recent match: " << matches[top].player1 << " vs " << matches[top].player2
             << ", Winner: " << matches[top].winner << endl;
    }
};
class ResultLog {
    char stack[MAX_RESULTS][100];  // For recent results (LIFO)
    char queue[MAX_RESULTS][100];  // For full history (FIFO)
    int stackTop, qFront, qRear;

public:
    ResultLog() {
        stackTop = -1;
        qFront = 0;
        qRear = -1;
    }

    void logResult(const char* result) {
        if (stackTop < MAX_RESULTS - 1) {
            stackTop++;
            strcpy(stack[stackTop], result);
        } else {
            cout << "Recent result stack is full.\n";
        }

        if ((qRear + 1) % MAX_RESULTS != qFront || qRear == -1) {
            qRear = (qRear + 1) % MAX_RESULTS;
            strcpy(queue[qRear], result);
        } else {
            cout << "History queue is full.\n";
        }

        cout << "Result logged: " << result << endl;
    }

    void showRecentResults() {
        cout << "\n--- Recent Results (LIFO Stack) ---\n";
        if (stackTop == -1) {
            cout << "No recent results.\n";
            return;
        }
        for (int i = stackTop; i >= 0; i--) {
            cout << stack[i] << endl;
        }
    }

    void showAllResults() {
        cout << "\n--- Match History (FIFO Queue) ---\n";
        if (qRear == -1) {
            cout << "No results in history.\n";
            return;
        }
        int i = qFront;
        while (true) {
            cout << queue[i] << endl;
            if (i == qRear) break;
            i = (i + 1) % MAX_RESULTS;
        }
    }
};
// ---------- PlayerQueue with Priority Enqueue ----------
class PlayerQueue {
    Player queue[MAX_PLAYERS];
    int size;

public:
    PlayerQueue() { size = 0; }

    bool isFull() { return size == MAX_PLAYERS; }
    bool isEmpty() { return size == 0; }

    int getPriority(const Player& p) {
        if (p.isEarlyBird) return 1;
        if (p.isWildcard) return 2;
        return 3;
    }

    void enqueue(Player p) {
        if (isFull()) {
            cout << "Queue is full.\n";
            return;
        }

        int i = size - 1;
        while (i >= 0 && getPriority(p) < getPriority(queue[i])) {
            queue[i + 1] = queue[i];
            i--;
        }
        queue[i + 1] = p;
        size++;
        cout << p.name << " registered successfully.\n";
    }

    void checkInPlayer(const char* playerName) {
        for (int i = 0; i < size; i++) {
            if (strcmp(queue[i].name, playerName) == 0) {
                queue[i].isCheckedIn = true;
                cout << playerName << " checked in successfully.\n";
                return;
            }
        }
        cout << "Player not found.\n";
    }

    void withdrawAndReplace(const char* playerName) {
        for (int i = 0; i < size; i++) {
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

                for (int j = i; j < size - 1; j++)
                    queue[j] = queue[j + 1];
                size--;

                enqueue(replacement);

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
        for (int i = 0; i < size; i++) {
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
        for (int i = 0; i < size; i++) {
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
        for (int i = 0; i < size; i++) {
            if (!queue[i].isCheckedIn) count++;
        }
        return count;
    }

    void viewPlayerProfile(const char* playerName) {
        for (int i = 0; i < size; i++) {
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

// ---------- Spectator Circular Queue ----------
class SpectatorQueue {
    Spectator queue[MAX_SPECTATORS];
    int front, rear, count;

public:
    SpectatorQueue() {
        front = 0;
        rear = -1;
        count = 0;
    }

    bool isFull() { return count == MAX_SPECTATORS; }
    bool isEmpty() { return count == 0; }

    void enqueue(Spectator s) {
        if (isFull()) {
            cout << "Spectator queue is full.\n";
            return;
        }
        rear = (rear + 1) % MAX_SPECTATORS;
        queue[rear] = s;
        count++;
        cout << s.name << " added to spectator queue.\n";
    }

    void dequeue() {
        if (isEmpty()) {
            cout << "No spectators to remove.\n";
            return;
        }
        cout << queue[front].name << " has left the viewing area.\n";
        front = (front + 1) % MAX_SPECTATORS;
        count--;
    }

    void displaySpectators() {
        cout << "\n--- Spectator Queue ---\n";
        if (isEmpty()) {
            cout << "No spectators in the queue.\n";
            return;
        }
        for (int i = 0; i < count; i++) {
            int index = (front + i) % MAX_SPECTATORS;
            cout << i + 1 << ". " << queue[index].name << endl;
        }
    }
};

// ---------- Livestream ----------
class Livestream {
    bool isLive;

public:
    Livestream() { isLive = false; }

    void startStream() {
        if (!isLive) {
            isLive = true;
            cout << "\n\u25CF Livestream has started!\n";
        } else {
            cout << "Livestream is already running.\n";
        }
    }

    void endStream() {
        if (isLive) {
            isLive = false;
            cout << "\n\u23F9 Livestream has ended.\n";
        } else {
            cout << "Livestream is not currently active.\n";
        }
    }

    void streamStatus() {
        cout << "Livestream is currently: " << (isLive ? "LIVE \u25CF" : "OFFLINE \u23F9") << endl;
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
int getIntInput(const string& prompt) {
    int value;
    cout << prompt;
    cin >> value;
    while (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Try again: ";
        cin >> value;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline
    return value;
}


int main() {
    PlayerQueue pq;
    SpectatorQueue sq;
    Livestream stream;
    MatchStack matchStack;
    ResultLog resultLog;

    int userType;
    do {
        cout << "\n=== APU ESPORT SYSTEM ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Player Access\n";
        cout << "3. Exit\n";
        cout << "4. Spectator Registration\n";
        cout << "5. Livestream Controls\n";
        userType = getIntInput("Choose Option: ");


        switch (userType) {
            case 1:
                if (adminLogin()) {
                    int choice;
                    char tempName[50];
                    do {
                        cout << "\n--- ADMIN PANEL ---\n";
                        cout << "1. Register Player\n";
                        cout << "2. Last-Minute Check-In\n";
                        cout << "3. Withdraw and Replace Player\n";
                        cout << "4. View All Registered Players\n";
                        cout << "5. Start Tournament\n";
                        cout << "6. View Spectator Queue\n";
                        cout << "7. Remove Spectator (End Session)\n";
                        cout << "8. Record a Match Result\n";
                        cout << "9. View Recent Match (Stack)\n";
                        cout << "10. View All Match History (Queue)\n";
                        cout << "11. Back to Main Menu\n";
                        cout << "Choose an option: ";
                        cin >> choice;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        switch (choice) {
                            case 1:
                                pq.enqueue(createPlayer());
                                pauseScreen();
                                break;

                            case 2:
                                cout << "Enter player name to check-in: ";
                                cin.getline(tempName, 50);
                                pq.checkInPlayer(tempName);
                                pauseScreen();
                                break;

                            case 3:
                                cout << "Enter player name to withdraw and replace: ";
                                cin.getline(tempName, 50);
                                pq.withdrawAndReplace(tempName);
                                pauseScreen();
                                break;

                            case 4:
                                pq.displayQueue();
                                pauseScreen();
                                break;

                            case 5:
                                if (pq.countUnchecked() > 0) {
                                    char confirm[10];
                                    cout << "\nWarning: " << pq.countUnchecked() << " player(s) not checked in.\nStart tournament anyway? (yes/no): ";
                                    cin >> confirm;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    if (strcmp(confirm, "yes") != 0) break;
                                }
                                cout << "\nTournament is starting with the following players:\n";
                                pq.displayCheckedInPlayers();
                                pauseScreen();
                                break;

                            case 6:
                                sq.displaySpectators();
                                pauseScreen();
                                break;

                            case 7:
                                sq.dequeue();
                                pauseScreen();
                                break;

                            case 8: {
                                char p1[50], p2[50], winner[50];
                                cout << "Enter Player 1: "; cin.getline(p1, 50);
                                cout << "Enter Player 2: "; cin.getline(p2, 50);
                                cout << "Enter Winner: "; cin.getline(winner, 50);
                                matchStack.pushMatch(p1, p2, winner);
                                char result[100];
                                sprintf(result, "%s defeated %s", winner, strcmp(winner, p1) == 0 ? p2 : p1);
                                resultLog.logResult(result);
                                pauseScreen();
                                break;
                            }

                            case 9:
                                matchStack.viewTopMatch();
                                pauseScreen();
                                break;

                            case 10:
                                resultLog.showAllResults();
                                pauseScreen();
                                break;

                            case 11:
                                break;

                            default:
                                cout << "Invalid option. Try again.\n";
                                pauseScreen();
                            }
                        } while (choice != 11);
                }
                break;

            case 2: {
                char name[50];
                cout << "\n--- PLAYER PANEL ---\n";
                cout << "Enter your name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.getline(name, 50);

                int choice;
                do {
                    cout << "\n1. View My Profile\n2. Check-In\n3. Back to Main Menu\nChoose an option: ";
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    switch (choice) {
                        case 1:
                            pq.viewPlayerProfile(name);
                            pauseScreen();
                            break;

                        case 2:
                            pq.checkInPlayer(name);
                            pauseScreen();
                            break;

                        case 3:
                            break;

                        default:
                            cout << "Invalid option.\n";
                            pauseScreen();
                    }
                } while (choice != 3);
                break;
            }


            case 3:
                cout << "Exiting system...\n";
                break;

            case 4: {
                Spectator s;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter spectator name: ";
                cin.getline(s.name, 50);
                sq.enqueue(s);
                break;
            }cin >> userType;
            

            case 5: {
                int streamChoice;
                do {
                    cout << "\n--- LIVESTREAM CONTROL ---\n";
                    cout << "1. Start Livestream\n2. End Livestream\n3. Stream Status\n4. Back\nChoose: ";
                    cin >> streamChoice;

                    switch (streamChoice) {
                        case 1:
                            stream.startStream();
                            pauseScreen();
                            break;

                        case 2:
                            stream.endStream();
                            pauseScreen();
                            break;

                        case 3:
                            stream.streamStatus();
                            pauseScreen();
                            break;

                        case 4:
                            break;

                        default:
                            cout << "Invalid option.\n";
                            pauseScreen();
                    }
                } while (streamChoice != 4);
                break;
            }
            default:
                cout << "Invalid option. Try again.\n";
            }
        } while (userType != 3);
        return 0;
    }
