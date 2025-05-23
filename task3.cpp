#include <iostream>
#include <queue>
#include <string>
using namespace std;

struct Spectator {
    string name;
    string type; 
};

queue<Spectator> vipQueue;
queue<Spectator> influencerQueue;
queue<Spectator> generalQueue;

void addSpectator(const string& name, const string& type) {
    Spectator s = {name, type};
    if (type == "VIP") vipQueue.push(s);
    else if (type == "Influencer") influencerQueue.push(s);
    else generalQueue.push(s);
    cout << type << " spectator " << name << " added to queue.\n";
}

void assignSeats(int vipSeats, int influencerSeats, int generalSeats) {
    cout << "\n=== Assigning Seats ===\n";

    cout << "\nVIP Seats:\n";
    for (int i = 0; i < vipSeats && !vipQueue.empty(); i++) {
        cout << "  - " << vipQueue.front().name << endl;
        vipQueue.pop();
    }

    cout << "\nInfluencer Seats:\n";
    for (int i = 0; i < influencerSeats && !influencerQueue.empty(); i++) {
        cout << "  - " << influencerQueue.front().name << endl;
        influencerQueue.pop();
    }

    cout << "\nGeneral Spectator Seats:\n";
    for (int i = 0; i < generalSeats && !generalQueue.empty(); i++) {
        cout << "  - " << generalQueue.front().name << endl;
        generalQueue.pop();
    }

    cout << "\nViewing slots have been filled.\n";
}

void displayQueues() {
    cout << "\nQueue Sizes:\n";
    cout << "  VIP: " << vipQueue.size() << "\n";
    cout << "  Influencer: " << influencerQueue.size() << "\n";
    cout << "  General: " << generalQueue.size() << "\n";
}

int main() {
    addSpectator("Alice", "VIP");
    addSpectator("Bob", "Influencer");
    addSpectator("Charlie", "General");
    addSpectator("Dave", "VIP");

    displayQueues();
    assignSeats(2, 1, 2);

    return 0;
}
