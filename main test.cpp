#include <iostream>
#include <cstring>
#include <limits>
using namespace std;

const int MAX_PLAYERS = 100;
const int MAX_SPECTATORS = 100;
const int MAX_MATCHES = 100;
const int MAX_RESULTS = 100;

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
    int player1Score;
    int player2Score;
    time_t matchTime;
};

class MatchStack {
    Match matches[MAX_MATCHES];
    int top;

public:
    MatchStack() { top = -1; }

    bool isFull() { return top == MAX_MATCHES - 1; }
    bool isEmpty() { return top == -1; }

    void pushMatch(const char* p1, const char* p2, const char* winner, int p1Score, int p2Score) {
        if (isFull()) {
            cout << "Match stack full.\n";
            return;
        }
        top++;
        strcpy(matches[top].player1, p1);
        strcpy(matches[top].player2, p2);
        strcpy(matches[top].winner, winner);
        matches[top].player1Score = p1Score;
        matches[top].player2Score = p2Score;
        matches[top].matchTime = time(nullptr);
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
    void saveToFile(const char* filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        for (int i = 0; i < size; i++) {
            file << queue[i].name << ","
                << queue[i].isEarlyBird << ","
                << queue[i].isWildcard << ","
                << queue[i].isCheckedIn << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No existing data file found.\n";
            return;
        }
        size = 0;
        string line;
        while (getline(file, line) && size < MAX_PLAYERS) {
            stringstream ss(line);
            string token;
            
            getline(ss, token, ',');
            strncpy(queue[size].name, token.c_str(), 49);
            queue[size].name[49] = '\0';
            
            getline(ss, token, ',');
            queue[size].isEarlyBird = token == "1";
            
            getline(ss, token, ',');
            queue[size].isWildcard = token == "1";
            
            getline(ss, token);
            queue[size].isCheckedIn = token == "1";
            
            size++;
        }
        file.close();
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
    void saveToFile(const char* filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        for (int i = 0; i < size; i++) {
            file << queue[i].name << ","
                << queue[i].isEarlyBird << ","
                << queue[i].isWildcard << ","
                << queue[i].isCheckedIn << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No existing data file found.\n";
            return;
        }
        size = 0;
        string line;
        while (getline(file, line) && size < MAX_PLAYERS) {
            stringstream ss(line);
            string token;
            
            getline(ss, token, ',');
            strncpy(queue[size].name, token.c_str(), 49);
            queue[size].name[49] = '\0';
            
            getline(ss, token, ',');
            queue[size].isEarlyBird = token == "1";
            
            getline(ss, token, ',');
            queue[size].isWildcard = token == "1";
            
            getline(ss, token);
            queue[size].isCheckedIn = token == "1";
            
            size++;
        }
        file.close();
    }

};
// Utility functions for input validation
bool validateName(const char* name) {
    if (strlen(name) == 0 || strlen(name) > 49) return false;
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isalnum(name[i])) return false;
    }
    return true;
}

bool validateYesNo(const char* input) {
    return strcmp(input, "yes") == 0 || strcmp(input, "no") == 0;
}

bool validateChoice(int choice, int min, int max) {
    return choice >= min && choice <= max;
}
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
    void saveToFile(const char* filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        for (int i = 0; i < size; i++) {
            file << queue[i].name << ","
                << queue[i].isEarlyBird << ","
                << queue[i].isWildcard << ","
                << queue[i].isCheckedIn << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No existing data file found.\n";
            return;
        }
        size = 0;
        string line;
        while (getline(file, line) && size < MAX_PLAYERS) {
            stringstream ss(line);
            string token;
            
            getline(ss, token, ',');
            strncpy(queue[size].name, token.c_str(), 49);
            queue[size].name[49] = '\0';
            
            getline(ss, token, ',');
            queue[size].isEarlyBird = token == "1";
            
            getline(ss, token, ',');
            queue[size].isWildcard = token == "1";
            
            getline(ss, token);
            queue[size].isCheckedIn = token == "1";
            
            size++;
        }
        file.close();
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
    cout << "Enter Admin Username: ";
    cin.getline(inputUser, 50);
    cout << "Enter Admin Password: ";
    cin.getline(inputPass, 50);

    if (strcmp(inputUser, USERNAME) == 0 && strcmp(inputPass, PASSWORD) == 0) {
        cout << "Login successful.\n";
        return true;
    }
    cout << "Invalid login. Returning to main menu.\n";
    return false;
}
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
    void saveToFile(const char* filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for writing.\n";
            return;
        }
        for (int i = 0; i < size; i++) {
            file << queue[i].name << ","
                << queue[i].isEarlyBird << ","
                << queue[i].isWildcard << ","
                << queue[i].isCheckedIn << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No existing data file found.\n";
            return;
        }
        size = 0;
        string line;
        while (getline(file, line) && size < MAX_PLAYERS) {
            stringstream ss(line);
            string token;
            
            getline(ss, token, ',');
            strncpy(queue[size].name, token.c_str(), 49);
            queue[size].name[49] = '\0';
            
            getline(ss, token, ',');
            queue[size].isEarlyBird = token == "1";
            
            getline(ss, token, ',');
            queue[size].isWildcard = token == "1";
            
            getline(ss, token);
            queue[size].isCheckedIn = token == "1";
            
            size++;
        }
        file.close();
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

int main() {
    PlayerQueue pq;
    SpectatorQueue sq;
    MatchStack matchStack;
    Livestream stream;
    ResultLog resultLog;

    // Load saved data
    pq.loadFromFile("players.dat");
    sq.loadFromFile("spectators.dat");
    matchStack.loadFromFile("matches.dat");
    resultLog.loadFromFile("results.dat");

    int userType;
    do {
        cout << "\n=== APU ESPORT SYSTEM ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Player Access\n";
        cout << "3. Exit\n";
        cout << "4. Spectator Registration\n";
        cout << "5. Livestream Controls\n";
        cout << "6. View Recent Results\n";
        cout << "Choose user type: ";
        
        while (!(cin >> userType) || !validateChoice(userType, 1, 6)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Enter 1-6: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (userType) {
            case 1: {
                if (!adminLogin()) break;
                
                int choice;
                char tempName[50];
                do {
                    cout << "\n--- ADMIN PANEL ---\n";
                    cout << "1. Register Player\n";
                    cout << "2. Last-Minute Check-In\n";
                    cout << "3. Withdraw and Replace Player\n";
                    cout << "4. View All Registered Players\n";
                    cout << "5. Start Tournament\n";
                    cout << "6. Manage Spectators\n";
                    cout << "7. Record Match Result\n";
                    cout << "8. View Recent Match\n";
                    cout << "9. View All Match History\n";
                    cout << "10. Save All Data\n";
                    cout << "11. Back to Main Menu\n";
                    cout << "Choose an option: ";
                    
                    while (!(cin >> choice) || !validateChoice(choice, 1, 11)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid choice. Enter 1-11: ";
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    switch (choice) {
                        case 1: pq.enqueue(createPlayer()); break;
                        case 2: {
                            cout << "Enter player name to check-in: ";
                            cin.getline(tempName, 50);
                            if (!validateName(tempName)) {
                                cout << "Invalid name format.\n";
                                break;
                            }
                            pq.checkInPlayer(tempName);
                            break;
                        }
                        case 3: {
                            cout << "Enter player name to withdraw: ";
                            cin.getline(tempName, 50);
                            if (!validateName(tempName)) {
                                cout << "Invalid name format.\n";
                                break;
                            }
                            pq.withdrawAndReplace(tempName);
                            break;
                        }
                        case 4: pq.displayQueue(); break;
                        case 5: {
                            if (pq.countUnchecked() > 0) {
                                char confirm[10];
                                cout << "\nWarning: " << pq.countUnchecked() << " player(s) not checked in.\n";
                                cout << "Start tournament anyway? (yes/no): ";
                                cin >> confirm;
                                while (!validateYesNo(confirm)) {
                                    cout << "Please enter 'yes' or 'no': ";
                                    cin >> confirm;
                                }
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                if (strcmp(confirm, "yes") != 0) break;
                            }
                            cout << "\nTournament starting with checked-in players:\n";
                            pq.displayCheckedInPlayers();
                            break;
                        }
                        case 6: {
                            int specChoice;
                            do {
                                cout << "\n--- SPECTATOR MANAGEMENT ---\n";
                                cout << "1. View Spectator Queue\n";
                                cout << "2. Remove Next Spectator\n";
                                cout << "3. Back\n";
                                cout << "Choose: ";
                                while (!(cin >> specChoice) || !validateChoice(specChoice, 1, 3)) {
                                    cin.clear();
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                    cout << "Invalid choice. Enter 1-3: ";
                                }
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                switch (specChoice) {
                                    case 1: sq.displaySpectators(); break;
                                    case 2: sq.dequeue(); break;
                                }
                            } while (specChoice != 3);
                            break;
                        }
                        case 7: {
                            char p1[50], p2[50], winner[50];
                            int p1Score, p2Score;
                            
                            cout << "Enter Player 1: "; 
                            cin.getline(p1, 50);
                            if (!validateName(p1)) {
                                cout << "Invalid name format.\n";
                                break;
                            }
                            
                            cout << "Enter Player 2: ";
                            cin.getline(p2, 50);
                            if (!validateName(p2)) {
                                cout << "Invalid name format.\n";
                                break;
                            }
                            
                            cout << "Enter Player 1 Score: ";
                            while (!(cin >> p1Score) || p1Score < 0) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid score. Enter positive number: ";
                            }
                            
                            cout << "Enter Player 2 Score: ";
                            while (!(cin >> p2Score) || p2Score < 0) {
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                cout << "Invalid score. Enter positive number: ";
                            }
                            
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "Enter Winner: ";
                            cin.getline(winner, 50);
                            if (!validateName(winner)) {
                                cout << "Invalid name format.\n";
                                break;
                            }
                            
                            matchStack.pushMatch(p1, p2, winner, p1Score, p2Score);

                            char result[100];
                            sprintf(result, "%s defeated %s (%d-%d)", winner, 
                                   strcmp(winner, p1) == 0 ? p2 : p1,
                                   strcmp(winner, p1) == 0 ? p1Score : p2Score,
                                   strcmp(winner, p1) == 0 ? p2Score : p1Score);
                            resultLog.logResult(result);
                            break;
                        }
                        case 8: matchStack.viewTopMatch(); break;
                        case 9: resultLog.showAllResults(); break;
                        case 10: {
                            pq.saveToFile("players.dat");
                            sq.saveToFile("spectators.dat");
                            matchStack.saveToFile("matches.dat");
                            resultLog.saveToFile("results.dat");
                            cout << "All data saved successfully.\n";
                            break;
                        }
                    }
                } while (choice != 11);
                break;
            }
            case 2: {
                char name[50];
                cout << "\n--- PLAYER PANEL ---\n";
                cout << "Enter your name: ";
                cin.getline(name, 50);
                if (!validateName(name)) {
                    cout << "Invalid name format.\n";
                    break;
                }
                int choice;
                do {
                    cout << "\n1. View My Profile\n";
                    cout << "2. Check-In\n";
                    cout << "3. Back to Main Menu\n";
                    cout << "Choose an option: ";
                    while (!(cin >> choice) || !validateChoice(choice, 1, 3)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid choice. Enter 1-3: ";
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (choice == 1) pq.viewPlayerProfile(name);
                    else if (choice == 2) pq.checkInPlayer(name);
                } while (choice != 3);
                break;
            }
            case 3: {
                char confirm[10];
                cout << "Save data before exiting? (yes/no): ";
                cin >> confirm;
                while (!validateYesNo(confirm)) {
                    cout << "Please enter 'yes' or 'no': ";
                    cin >> confirm;
                }
                if (strcmp(confirm, "yes") == 0) {
                    pq.saveToFile("players.dat");
                    sq.saveToFile("spectators.dat");
                    matchStack.saveToFile("matches.dat");
                    resultLog.saveToFile("results.dat");
                }
                cout << "Exiting system...\n";
                break;
            }
            case 4: {
                Spectator s;
                cout << "Enter spectator name: ";
                cin.getline(s.name, 50);
                if (!validateName(s.name)) {
                    cout << "Invalid name format.\n";
                    break;
                }
                sq.enqueue(s);
                break;
            }
            case 5: {
                int streamChoice;
                do {
                    cout << "\n--- LIVESTREAM CONTROL ---\n";
                    cout << "1. Start Livestream\n";
                    cout << "2. End Livestream\n";
                    cout << "3. Stream Status\n";
                    cout << "4. Back\n";
                    cout << "Choose: ";
                    while (!(cin >> streamChoice) || !validateChoice(streamChoice, 1, 4)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid choice. Enter 1-4: ";
                    }
                    switch (streamChoice) {
                        case 1: stream.startStream(); break;
                        case 2: stream.endStream(); break;
                        case 3: stream.streamStatus(); break;
                    }
                } while (streamChoice != 4);
                break;
            }
            case 6: resultLog.showRecentResults(); break;
        }
    } while (userType != 3);

    return 0;
}