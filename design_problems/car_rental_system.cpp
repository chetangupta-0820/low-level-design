#include<bits/stdc++.h>
using namespace std;

class Vehicle{
public:
    enum class Status{
        AVAILABLE, 
        BOOKED, 
        MAINTANANCE
    };
    int registrationNumber;
    string brand, model;
    int year;
    double baseRental;
    Status status = Status::AVAILABLE;

    virtual ~Vehicle() = default;
    Vehicle() = default;
    Vehicle(int number, int year, string brand, string model, double baseRental): registrationNumber(number), year(year), 
                                                                        brand(brand), model(model), baseRental(baseRental) { }
    virtual double getRentalCost(int days) = 0;
};

class SUVCar: public Vehicle{
    int baseMultiplier = 0.75;
public:
    SUVCar(int number, int year, string brand, string model, double baseRental,int baseMultiplier): Vehicle(number, year, brand, model, baseRental), baseMultiplier(baseMultiplier) {}

    double getRentalCost(int days){
        return baseRental* baseMultiplier* days;
    }
};

class Bike: public Vehicle{
    int baseMultiplier = 0.35;
public:
    Bike(int number, int year, string brand, string model, double baseRental,int baseMultiplier): Vehicle(number, year, brand, model, baseRental), baseMultiplier(baseMultiplier) {}

    double getRentalCost(int days){
        return baseRental* baseMultiplier* days;
    }
};

class Location{
public:
    string address, city, state, country;
    int pincode; 
};

class RentalStore{
public:
    Location location;
    map<int, shared_ptr<Vehicle>> vehicles;

    void addVehicle(int number, shared_ptr<Vehicle> vehicle){
        vehicles[number] = vehicle;
    }

    void removeVehicle(int number){
        vehicles.erase(number);
    }

    shared_ptr<Vehicle> getVehicle(int number){
        if(vehicles.count(number))
            return vehicles[number];

        throw logic_error("Vehicle with given number doesnt exist");
    }

    vector<shared_ptr<Vehicle>> getAvailableVehicles(){
        vector<shared_ptr<Vehicle>> result;
        for(auto &[number, vehicle]: vehicles){
            if(vehicle->status == Vehicle::Status::AVAILABLE)
                result.push_back(vehicle);
        }

        return result;
    }    
};

class User{
public:
    string name;
    int phone;
    string address;
};

class Reservation{
public:
    enum class Status{
        PENDING, 
        CONFIRMED, 
        CANCELLED
    };
    User user;
    RentalStore pickupStore;
    RentalStore dropStore;
    int startDate, endDate;
    shared_ptr<Vehicle> vehicle;
    Status status = Status::PENDING;
    double amount;

    Reservation() = default;
    Reservation( User user, RentalStore pickupStore, RentalStore dropStore, int startDate,int endDate,shared_ptr<Vehicle> vehicle): user(user), pickupStore(pickupStore), dropStore(dropStore), 
                startDate(startDate), endDate(endDate), vehicle(vehicle) {}

    void markConfirm(){
        status = Status::CONFIRMED;
        vehicle->status = Vehicle::Status::BOOKED;
    }

    void markCancel(){
        status = Status::CANCELLED;
        vehicle->status = Vehicle::Status::AVAILABLE;
    }

    double getAmount(){
        return vehicle->getRentalCost(endDate-startDate);
    }
};

class ReservationManager{
    int counter = 0;
public:
    map<int, Reservation> reservations;
    // TODO: Add a map of vehicleId, and booked slots for it 

    int addReservation(User user, RentalStore pickupStore, RentalStore dropStore, int startDate,int endDate,shared_ptr<Vehicle> vehicle){
        int reservationId = counter++;
        Reservation reservation = Reservation(user, pickupStore, dropStore, startDate, endDate, vehicle);
        reservations[reservationId] = reservation;
        return reservationId;
    }

    double getAmount(int reservationId){
        return reservations[reservationId].getAmount();
    }

    void cancelReservation(int reservationId){
        reservations[reservationId].markCancel();
    }

    void confirmReservation(int reservationId){
        reservations[reservationId].markConfirm();
    }
};

class PaymentProcessor{
public:
    virtual ~PaymentProcessor() = default;
    virtual bool pay(double amount) = 0;
};

class CreditCardProcessor:public PaymentProcessor{
    string number;
public:
    CreditCardProcessor(string number): number(number){}

    bool pay(double amount){
        cout<<"Deducting "<<amount<<" from credit card "<<number<<endl;
    }
};

class RentalSystem{
    int counter=0;
    int ucounter=0;
public:
    shared_ptr<PaymentProcessor> paymentProcessor;
    shared_ptr<ReservationManager> reservationManager;
    map<int,shared_ptr<RentalStore>> stores;
    map<int, User> users;

    static shared_ptr<RentalSystem> getInstance(){
        static auto instance = make_shared<RentalSystem>(new RentalSystem());

        return instance;
    }

    int addUser(User &user){
        users[ucounter++] = user;
        return ucounter;
    }

    void addStore(shared_ptr<RentalStore> store){
        stores[counter++] = store;
    }

    void removeStore(int id){
        if(stores.count(id))
            stores.erase(id);
    }

    vector<shared_ptr<Vehicle>> getAvailableVehicles(){
        vector<shared_ptr<Vehicle>> res;
        for(auto [id, store]:stores){
            auto temp = store->getAvailableVehicles();
            res.insert(res.end(),temp.begin(), temp.end());
        }
    }
    
    int makeReservation(int userId, RentalStore pickupStore, RentalStore dropStore, int startDate,int endDate,shared_ptr<Vehicle> vehicle){
        int reservationId = reservationManager->addReservation(users[userId], pickupStore, dropStore, startDate, endDate, vehicle);
        if(paymentProcessor->pay(reservationManager->getAmount(reservationId)))
            reservationManager->confirmReservation(reservationId);
        else    
            reservationManager->cancelReservation(reservationId);
    }

};


int main(){


    return 0;
}