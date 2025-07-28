#include<bits/stdc++.h>
using namespace std;

enum class RoomType{
    SMALL,
    LARGE
};

class Room{
public:
    int id;
    RoomType type;
    int capacity;
    Room() = default;
    Room(int id, RoomType type, int capacity) : id(id), type(type), capacity(capacity) {}
};

class Employee{
public:
    Employee() = default;
    int id;
    string name;
    string department;
    
    Employee(int id, string name, string department)
    : id(id), name(name), department(department) {}
};

class Booking{
public:
    string id;
    int roomId;
    string employeeName;
    int slotNumber;
    int attendees;
    Booking() = default;
    Booking(string id, int roomId, string employeeName, int slotNumber, int attendees)
        : id(id), roomId(roomId), employeeName(employeeName), slotNumber(slotNumber), attendees(attendees){}
};

class Scheduler{
public:
    virtual Booking& bookRoomHelper(string empName, int attendees, int slot, vector<int>& candidate) = 0;
    virtual vector<Booking> getBookingsByEmployee(string &name) = 0;
    virtual vector<Booking> getBookingsByRoom(int roomId) = 0;
    virtual ~Scheduler() = default;

};

class OptimalScheduler: public Scheduler{
    map<string, Booking> bookings;
    map<int, set<int>> roomBookings; // roomId -> {booked slots}
    unordered_map<string, int> idNamespace;
    
    string generateBookingId(string name){
        string id = name.substr(0, 3);
        transform(id.begin(), id.end(), id.begin(), ::toupper);
        id += to_string(idNamespace[id]++);

        return id;
    }

public:
    Booking& bookRoomHelper(string empName, int attendees, int slot, vector<int>& candidate){
        if(slot <1 || slot> 10)
            throw logic_error("Invalid Slot");
            
        for(auto roomId: candidate){
            auto it = roomBookings[roomId].find(slot);
            if(it != roomBookings[roomId].end()) // already booked
                continue;
            
            string bookingId = generateBookingId(empName);
            bookings[bookingId] = Booking(bookingId, roomId, empName, slot, attendees);
            roomBookings[roomId].insert(slot);

            return bookings[bookingId];
        }

        throw runtime_error("No suitable room free for this slot");
    }

    vector<Booking> getBookingsByEmployee(string &name) {
        vector<Booking> res;
        for (const auto &[id, b] : bookings){
            if (b.employeeName == name)
                res.push_back(b);
        } 
        return res;
    }
    vector<Booking> getBookingsByRoom(int roomId) {
        vector<Booking> res;
        for (const auto &[id, b] : bookings){
            if (b.roomId == roomId) 
                res.push_back(b);
        } 
        return res;
    }

};

class Storage{
public:
    virtual int addEmployee(string name, string department) = 0;
    virtual int addRoom(RoomType type, int capacity) = 0;
    virtual bool roomExists(int id) = 0;
    virtual vector<int> getCandidateRooms(int reqCapacity) = 0;
};

class InMemoryStorage: public Storage{
    int employeeIdCounter = 0;
    int roomIdCounter = 0;
    unordered_map<int, Room> rooms;
    unordered_map<string, Employee> employees; 
public:
    int addEmployee(string name, string department){
        if(employees.count(name))
            return employees[name].id;
        
        int id = employeeIdCounter++;
        employees[name] = Employee(id, name, department);
        return id;
    }

    int addRoom(RoomType type, int capacity){
        int id = roomIdCounter++;
        rooms[id] = Room(id, type, capacity);
        return id;
    }

    vector<int> getCandidateRooms(int reqCapacity){
        vector<int> res;
        for(auto& [id, room]: rooms){
            if(room.capacity >= reqCapacity)
                res.push_back(id);
        }

        sort(res.begin(), res.end(), [this](int a, int b) { return rooms[a].capacity < rooms[b].capacity; });
        return res;            
    }

    bool roomExists(int id){ 
        return rooms.count(id); 
    }
};

// singleton
class BookingSystem{
    shared_ptr<Storage> storage;
    shared_ptr<Scheduler> scheduler;
    BookingSystem()
    : storage(make_shared<InMemoryStorage>()), scheduler(make_shared<OptimalScheduler>()) {}

    bool isDigits(const string &s) const { return !s.empty() && all_of(s.begin(), s.end(), ::isdigit); }

public:
    static shared_ptr<BookingSystem> getInstance(){
        static shared_ptr<BookingSystem> instance = shared_ptr<BookingSystem>(new BookingSystem);
        return instance;
    }

    int addEmployee(const string name, const string &dept) { 
        return storage->addEmployee(name, dept); 
    }
    int addRoom(RoomType type, int capacity){ 
        return storage->addRoom(type, capacity); 
    }

    void bookRoom(string empName, int attendees, int slot) {
        vector<int> candidates = storage->getCandidateRooms(attendees);
        try{
            auto booking = scheduler->bookRoomHelper(empName, attendees, slot, candidates);
            cout<<"Booking successful for Room "<<booking.roomId<<". Slot "<<slot<<". Booking ID: "<<booking.id<<endl;
        }
        catch(...){
            cout<<"No suitable room available for given criteria"<<endl;
        }        
    }

    void viewSchedule(string target) const {
        if (isDigits(target)) {
            int roomId = stoi(target);
            if (!storage->roomExists(roomId)) {
                cout << "Room ID " << roomId << " does not exist.\n";
                return;
            }
            auto list = scheduler->getBookingsByRoom(roomId);
            cout << "Schedule for Room " << roomId << ":\n";
            for (int s = 1; s <= 10; ++s) {
                auto it = find_if(list.begin(), list.end(), [&](const Booking &b){return b.slotNumber==s;});
                if (it == list.end()) cout << "  Slot " << s << ": Available\n";
                else cout << "  Slot " << s << ": Booked by " << it->employeeName << " (" << it->id << ")\n";
            }
            return;
        }
        auto list = scheduler->getBookingsByEmployee(target);
        cout << "Bookings for " << target << ":\n";
        if (list.empty()) {
            cout << "  None\n";
            return;
        }
        for (const auto &b : list) cout << "  " << b.id << ": Room " << b.roomId << " Slot " << b.slotNumber << " (" << b.attendees << " attendees)\n";
    }


};

int main(){

    auto sys = BookingSystem::getInstance();

    sys->addRoom(RoomType::SMALL, 8);   // 0
    sys->addRoom(RoomType::LARGE, 20);  // 1

    sys->addEmployee("Alice", "Marketing");
    sys->addEmployee("Bob", "Sales");
    sys->addEmployee("Charlie", "Engineering");

    cout << "\nPositive bookings\n";
    sys->bookRoom("Alice", 5, 2);   // success
    sys->bookRoom("Bob", 15, 2);       // slot full
    sys->bookRoom("Charlie", 5, 4); // success

    cout << "\nNegative bookings\n";
    sys->bookRoom("Charlie", 30, 3);  // capacity too high
    sys->bookRoom("Bob", 6, 2);       // slot full
    sys->bookRoom("Alice", 4, 11);    // invalid slot

    cout << "\nRoom schedules\n";
    sys->viewSchedule("0");
    cout << '\n';
    sys->viewSchedule("1");

    cout << "\nEmployee schedules\n";
    sys->viewSchedule("Alice");
    cout << '\n';
    sys->viewSchedule("Charlie");
    cout << '\n';
    sys->viewSchedule("Bob");

    cout << "\nNon-existent room\n";
    sys->viewSchedule("5");

    return 0;
}