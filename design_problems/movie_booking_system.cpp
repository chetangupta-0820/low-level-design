#include<bits/stdc++.h>
using namespace std;

class Movie{
public:
    int movieId;
    string name;
    int duration;
};

class Seat{
public:
    int id;
    int row;
    string category;
};

class Screen{
public:
    int capacity;
    string name;
    int theatre_id;
    vector<Seat> seats;
};

class Theatre{
public:
    int id;
    string name;
    string location;
    vector<Screen> screens;
};

class Show{
public:
    int id;
    int date;
    int startTime, endTime;
    Movie movie;
    Screen screen;
}; 

class User{
    string name;
    string email;
    int mobile;
    int age;
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
    int seat_id;
    User user;
    int createdAt;
    int validFor;
};

class iSeatLockProvider{
public:
    virtual ~iSeatLockProvider(){}
    virtual bool lockSeats(int showId, vector<Seat> seats) = 0;
    virtual void unlockSeats(int showId, vector<Seat> seats) = 0;
    virtual vector<int> getLockedSeats(int showId) = 0;
};

// singleton
class SeatLockProvider: public iSeatLockProvider{
    // show_id - > (seat_id, seat lock) 
    unordered_map<int, unordered_map<int, SeatLock>> locks;
    unordered_map<int, mutex> mtx;
    mutex gmtx;
    thread bgt;
    bool stop_bgt;
    
    void worker(){
        while(!stop_bgt){
            for(auto& [show_id, mp]: locks){
                {   
                    unique_lock<mutex> lock(mtx[show_id]);
                    auto ctime = time(0);
                    for(auto it = mp.begin(); it != mp.end(); ++it){
                        auto& [seat_id, lock] = *it;
                        if((ctime - lock.createdAt)< lock.validFor){
                               it = mp.erase(it);
                        }
                    }
                }
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }

public:
    SeatLockProvider(){
        stop_bgt = false;
        bgt = thread(worker, this);
    }
    ~SeatLockProvider(){
        stop_bgt = true;
        if(bgt.joinable())
            bgt.join();
    }
    bool lockSeats(Show show, vector<Seat> seats){
        {
            unique_lock<mutex> glock(gmtx);
            mtx.try_emplace(show.id);
            locks.try_emplace(show.id);
        }

        auto& showLock = locks[show.id];
        {
            unique_lock<mutex> lock(mtx[show.id]);
            for(auto& seat:seats){
                if(showLock.count(seat.id))
                    return false;
            }
            auto currentTime = time(0);
            for(auto& seat:seats){
                showLock[seat.id] = SeatLock();
            }
        }
        return true;
    }

    void unlockSets(Show show, vector<Seat> seats){
        auto& showLock = locks[show.id];
        if(showLock.empty())
            return;
        {
            unique_lock<mutex> lock(mtx[show.id]);
            for(auto& seat:seats){
                showLock.erase(seat.id);
            }
        }
    }

    vector<int> getLockedSeats(Show show);
};

// singleton
class SeatAvailabilityService{
    shared_ptr<BookingService> bookingService;
    shared_ptr<iSeatLockProvider> lockProvider;
public:
    SeatAvailabilityService(shared_ptr<iSeatLockProvider> lockProvider, shared_ptr<BookingService> bookingService)
        : lockProvider(lockProvider), bookingService(bookingService) {}
    vector<int> getAvaialableSeats(Show show); // show.screen.seats - getUnAvailableSeats
    vector<int> getUnAvaialableSeats(Show show); // bookingService.getBookedSeats - lockProvider.getLockedSeats
};

// singleton
class BookingService{
    // Booking Id -> Booking
    map<string, Booking> bookings;
    shared_ptr<PaymentStrategy> paymentStrategy;
    shared_ptr<iSeatLockProvider> lockProvider;
    atomic_int bookingIdCounter{0};

public:
    BookingService(shared_ptr<PaymentStrategy> paymentStrategy, shared_ptr<iSeatLockProvider> lockProvider) 
        : paymentStrategy(paymentStrategy), lockProvider(lockProvider) {}

    Booking getBooking(string bid);
    vector<Seat> getBookedSeats(Show show);
    
    Booking createBooking(Show show, vector<Seat> seats){
        // check if seats are already booked in booking map

        // try to lock seats
        
        // if true create booking object, update map and return 

        // else throw exception

    }
    
    bool confirmBooking(Booking booking){
        // update booking status to confirmed
    }
};

class PaymentStrategy{
public:
    virtual ~PaymentStrategy(){}
    virtual bool processPayment(int amount) = 0;
};

class CreditCard: public PaymentStrategy{
public:
    bool processPayment(int amount){
        return true;
    }
};

int main(){

    // create threatres, screens, movies, seats

    // create singleton booking service, seat availablity service, lock provider service

    // seat availablity service. get available seats

    // users selects seats -> booking service. create booking

    // if true -> process payment

    // if true -> confirm booking 

    // unhappy path -> someone else booked same seats, retry -> selection 

    // payment failed retry till seat are locked

    return 0;
}