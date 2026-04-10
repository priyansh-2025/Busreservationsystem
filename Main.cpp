#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

// ───────── Route ─────────
class Route {
private:
    string routeID, from, to;
    double fare;
    int seatCapacity;
    set<int> bookedSeats;

public:
    Route(string id, string f, string t, double fare, int cap)
        : routeID(id), from(f), to(t), fare(fare), seatCapacity(cap) {}

    string getRouteID() const { return routeID; }
    string getFrom() const { return from; }
    string getTo() const { return to; }
    double getFare() const { return fare; }
    int getCapacity() const { return seatCapacity; }
    int getAvailable() const { return seatCapacity - bookedSeats.size(); }

    bool isSeatAvailable(int seat) const {
        return seat >= 1 && seat <= seatCapacity &&
               bookedSeats.find(seat) == bookedSeats.end();
    }

    void bookSeat(int seat) {
        if (!isSeatAvailable(seat))
            throw invalid_argument("Seat unavailable\n");
        bookedSeats.insert(seat);
    }

    void releaseSeat(int seat) {
        bookedSeats.erase(seat);
    }

    void display() const {
        cout << left
             << setw(10) << routeID
             << setw(14) << from
             << setw(14) << to
             << setw(10) << fixed << setprecision(2) << fare
             << setw(10) << seatCapacity
             << getAvailable() << "\n";
    }
};

// ───────── Ticket ─────────
class Ticket {
private:
    string ticketID, passengerName, routeID;
    vector<int> seats;
    bool cancelled;

public:
    Ticket(string tid, string name, string rid, vector<int> s)
        : ticketID(tid), passengerName(name), routeID(rid), seats(s), cancelled(false) {}

    string getTicketID() const { return ticketID; }
    string getPassenger() const { return passengerName; }
    string getRouteID() const { return routeID; }
    vector<int> getSeats() const { return seats; }
    bool isCancelled() const { return cancelled; }

    void cancel() { cancelled = true; }

    void display() const {
        cout << "\nTicket ID: " << ticketID
             << "\nPassenger: " << passengerName
             << "\nRoute: " << routeID
             << "\nSeats: ";
        for (int s : seats) cout << s << " ";
        cout << "\nStatus: " << (cancelled ? "CANCELLED" : "ACTIVE") << "\n";
    }
};

// ───────── System ─────────
class ReservationSystem {
private:
    map<string, Route> routes;
    map<string, Ticket> tickets;
    int counter = 1000;

    string generateID() {
        return "TKT" + to_string(++counter);
    }

public:
    void addRoute(string id, string from, string to, double fare, int cap) {
        routes.emplace(id, Route(id, from, to, fare, cap));
    }

    void displayRoutes() {
        cout << "\nRoutes:\n";
        for (auto &p : routes) p.second.display();
    }

    string book(string name, string rid, vector<int> seats) {
        if (!routes.count(rid)) throw invalid_argument("Invalid route\n");

        Route &r = routes.at(rid);

        for (int s : seats)
            if (!r.isSeatAvailable(s))
                throw invalid_argument("Seat unavailable\n");

        for (int s : seats) r.bookSeat(s);

        string id = generateID();
        tickets.emplace(id, Ticket(id, name, rid, seats));

        cout << "Booked! Ticket ID: " << id << "\n";
        return id;
    }

    void cancel(string tid) {
        if (!tickets.count(tid)) throw invalid_argument("Not found\n");

        Ticket &t = tickets.at(tid);

        if (t.isCancelled()) throw invalid_argument("Already cancelled\n");

        Route &r = routes.at(t.getRouteID());

        for (int s : t.getSeats()) r.releaseSeat(s);

        t.cancel();
        cout << "Cancelled\n";
    }

    void search(string tid) {
        if (!tickets.count(tid)) {
            cout << "Not found\n";
            return;
        }
        tickets.at(tid).display();
    }
};

// ───────── helper ─────────
vector<int> parseSeats(char *input) {
    vector<int> seats;
    char *token = strtok(input, ",");
    while (token) {
        seats.push_back(atoi(token));
        token = strtok(NULL, ",");
    }
    return seats;
}

// ───────── MAIN ─────────
int main() {
    ReservationSystem sys;

    sys.addRoute("R001", "Chennai", "Bangalore", 350, 30);
    sys.addRoute("R002", "Mumbai", "Pune", 150, 30);
    sys.addRoute("R003", "Delhi", "Jaipur", 250, 20);

    int choice;

    do {
        cout << "\n1.Display\n2.Book\n3.Cancel\n4.Search\n0.Exit\nChoice: ";
        cout.flush();

        scanf("%d", &choice);
        getchar();

        try {
            if (choice == 1) sys.displayRoutes();

            else if (choice == 2) {
                char name[100], rid[20], seatsStr[100];

                cout << "Name: "; cout.flush();
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                cout << "Route ID: "; cout.flush();
                fgets(rid, sizeof(rid), stdin);
                rid[strcspn(rid, "\n")] = 0;

                cout << "Seats (1,2,3): "; cout.flush();
                fgets(seatsStr, sizeof(seatsStr), stdin);
                seatsStr[strcspn(seatsStr, "\n")] = 0;

                vector<int> seats = parseSeats(seatsStr);
                sys.book(name, rid, seats);
            }

            else if (choice == 3) {
                char tid[20];
                cout << "Ticket ID: "; cout.flush();
                fgets(tid, sizeof(tid), stdin);
                tid[strcspn(tid, "\n")] = 0;

                sys.cancel(tid);
            }

            else if (choice == 4) {
                char tid[20];
                cout << "Ticket ID: "; cout.flush();
                fgets(tid, sizeof(tid), stdin);
                tid[strcspn(tid, "\n")] = 0;

                sys.search(tid);
            }

        } catch (exception &e) {
            cout << "Error: " << e.what();
        }

    } while (choice != 0);

    cout << "Goodbye!\n";
    cout.flush();
    return 0;
}