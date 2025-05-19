#include <iostream>
#include <cstring>
#include <limits>
#include <fstream>
#include <ctime>
using namespace std;

const int MAX_PLAYERS = 100;
const int MAX_SPECTATORS = 100;
const int MAX_MATCHES = 100;
const int MAX_RESULTS = 100;

// Input validation functions
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
    MatchStack() : top(-1) {}
    ~MatchStack() {} // Destructor placeholder

    bool isFull() const { return top == MAX_MATCHES - 1; }
    bool isEmpty() const { return top == -1; }

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

    void viewTopMatch() const {
        if (isEmpty()) {
            cout << "No recent match.\n";
            return;
        }
        cout << "Recent match: " << matches[top].player1 << " vs " << matches[top].player2
             << ", Winner: " << matches[top].winner 
             << " (" << matches[top].player1Score << "-" << matches[top].player2Score << ")\n";
    }

    void saveToFile(const char* filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error saving matches.\n";
            return;
        }
        for (int i = 0; i <= top; i++) {
            file << matches[i].player1 << ","
                 << matches[i].player2 << ","
                 << matches[i].winner << ","
                 << matches[i].player1Score << ","
                 << matches[i].player2Score << ","
                 << matches[i].matchTime << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No match history found.\n";
            return;
        }
        top = -1;
        string line;
        while (getline(file, line) && top < MAX_MATCHES - 1) {
            top++;
            stringstream ss(line);
            string token;
            
            getline(ss, token, ',');
            strcpy(matches[top].player1, token.c_str());
            
            getline(ss, token, ',');
            strcpy(matches[top].player2, token.c_str());
            
            getline(ss, token, ',');
            strcpy(matches[top].winner, token.c_str());
            
            getline(ss, token, ',');
            matches[top].player1Score = stoi(token);
            
            getline(ss, token, ',');
            matches[top].player2Score = stoi(token);
            
            getline(ss, token);
            matches[top].matchTime = stol(token);
        }
        file.close();
    }
};

class ResultLog {
    char stack[MAX_RESULTS][100];
    char queue[MAX_RESULTS][100];
    int stackTop, qFront, qRear;

public:
    ResultLog() : stackTop(-1), qFront(0), qRear(-1) {}
    ~ResultLog() {} // Destructor placeholder

    void logResult(const char* result) {
        // Stack implementation
        if (stackTop < MAX_RESULTS - 1) {
            stackTop++;
            strcpy(stack[stackTop], result);
        }

        // Circular queue implementation
        if ((qRear + 1) % MAX_RESULTS != qFront || qRear == -1) {
            qRear = (qRear + 1) % MAX_RESULTS;
            strcpy(queue[qRear], result);
        }
    }

    void showRecentResults() const {
        cout << "\n--- Recent Results (LIFO Stack) ---\n";
        if (stackTop == -1) {
            cout << "No recent results.\n";
            return;
        }
        for (int i = stackTop; i >= 0; i--) {
            cout << stack[i] << endl;
        }
    }

    void showAllResults() const {
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

    void saveToFile(const char* filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error saving results.\n";
            return;
        }
        for (int i = 0; i <= stackTop; i++) {
            file << stack[i] << "\n";
        }
        file.close();
    }

    void loadFromFile(const char* filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "No result history found.\n";
            return;
        }
        stackTop = -1;
        string line;
        while (getline(file, line) && stackTop < MAX_RESULTS - 1) {
            stackTop++;
            strcpy(stack[stackTop], line.c_str());
        }
        file.close();
    }
};

class PlayerQueue {
    Player queue[MAX_PLAYERS];
    int size;

public:
    PlayerQueue() : size(0) {}
    ~PlayerQueue() {} // Destructor placeholder

    bool isFull() const { return size == MAX_PLAYERS; }
    bool isEmpty() const { return size == 0; }

    int getPriority(const Player& p) const {
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
                Player replacement;
                char early[10], wild[10];

                cout << "Enter replacement player name: ";
                cin.getline(replacement.name, 50);
                while (!validateName(replacement.name)) {
                    cout << "Invalid name. Use alphanumeric characters (max 49): ";
                    cin.getline(replacement.name, 50);
                }

                cout << "Is Early Bird? (yes/no): ";
                cin >> early;
                while (!validateYesNo(early)) {
                    cout << "Invalid input. Enter yes/no: ";
                    cin >> early;
                }

                cout << "Is Wildcard Entry? (yes/no): ";
                cin >> wild;
                while (!validateYesNo(wild)) {
                    cout << "Invalid input. Enter yes/no: ";
                    cin >> wild;
                }

                replacement.isEarlyBird = strcmp(early, "yes") == 0;
                replacement.isWildcard = strcmp(wild, "yes") == 0;
                replacement.isCheckedIn = false;

                // Remove old player
                for (int j = i; j < size - 1; j++)
                    queue[j] = queue[j + 1];
                size--;

                // Add replacement
                enqueue(replacement);
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }
        }
        cout << "Player not found.\n";
    }

    void displayQueue() const {
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

    void saveToFile(const char* filename) const {
        ofstream file(filename);
        if (!file) {
            cerr << "Error saving player data.\n";
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
            cerr << "No player data found.\n";
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

    // Other methods remain same as previous version...
};

// SpectatorQueue and Livestream classes remain similar with added save/load
// and const correctness (see previous implementation)

// Utility functions
Player createPlayer() {
    Player p;
    char early[10], wild[10];

    cout << "Enter player name: ";
    cin.getline(p.name, 50);
    while (!validateName(p.name)) {
        cout << "Invalid name. Use alphanumeric characters (max 49): ";
        cin.getline(p.name, 50);
    }

    cout << "Is Early Bird? (yes/no): ";
    cin >> early;
    while (!validateYesNo(early)) {
        cout << "Invalid input. Enter yes/no: ";
        cin >> early;
    }

    cout << "Is Wildcard Entry? (yes/no): ";
    cin >> wild;
    while (!validateYesNo(wild)) {
        cout << "Invalid input. Enter yes/no: ";
        cin >> wild;
    }

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

// Main function remains as in previous answer with all improvements

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

    // [Rest of main() implementation as shown in previous answer]
    // ... (main menu logic with all features)

    return 0;
}