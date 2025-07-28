#include<bits/stdc++.h>
using namespace std;

class Movie{
    int movieId;
    string name;
    int duration;
};

class Seat{
    int seatId;
    int row;
    string category;
};

class Screen;

class Theatre{
    int id;
    string name;
    string location;
    vector<Screen> screens;
};

class Screen{
    int capacity;
    string name;
    Theatre theatre;
    vector<Seat> seats;
};

class Show{
public:
    int id;
    int startTime, endTime;
    Movie movie;
    Screen screen;
}; 

class User{
    string name;
    string email;
    int mobile;
};

class Booking{
    enum class Status{
        CREATED,
        PENDING,
        CONFIRMED
    };
    int id;
    User user;
    Show show;
    vector<Seat> seats;
    double amount;
    Status status;
};

class SeatLock{
public:
    Seat seat;
    User user;
    int createdAt;
    int validFor;
};

// can be an interface
class SeatLockProvider{
    // show id - > (seat, seat lock) 
    unordered_map<int, unordered_map<Seat, SeatLock>> locks;
    unordered_map<int, mutex> mtx;
    thread bgt;
    
    void worker(){
        while(true){
            for(auto [show, mp]: locks){
                {   
                    unique_lock<mutex> lock(mtx[show]);
                    auto ctime = time(0);
                    for(auto& [seat, lock]:mp){
                        if((ctime - lock.createdAt)< lock.validFor){
                               mp.erase(seat);
                        }
                    }
                }
            }
        }
    }

public:
    SeatLockProvider(){
        bgt = thread(worker, this);
    }
    ~SeatLockProvider(){
        if(bgt.joinable())
            bgt.join();
    }
    bool lockSeats(Show show, vector<Seat> seats){
        auto showLock = locks[show.id];
        {
            unique_lock<mutex> lock(mtx[show.id]);
            for(auto& seat:seats){
                if(showLock.count(seat))
                    return false;
            }
            auto currentTime = time(0);
            for(auto& seat:seats){
                showLock[seat] = SeatLock();
            }
        }
        return true;
    }

    void unlockSets(Show show, vector<Seat> seats){
        auto showLock = locks[show.id];
        if(showLock.empty())
            return;
        {
            unique_lock<mutex> lock(mtx[show.id]);
            for(auto& seat:seats){
                showLock.erase(seat);
            }
        }
    }
};

class SeatAvailabilityService{
public:
    vector<Seat> getLockedSeats(Show show);
};

class BookingService{
    // Booking Id -> Booking
    map<string, Booking> bookings;
public:
    Booking getBooking(string bid);
    vector<Seat> getBookedSeats(Show show);
    void createBooking(Show show, vector<Seat> seats);
};


int main(){

    return 0;
}