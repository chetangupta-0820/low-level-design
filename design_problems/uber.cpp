#include<bits/stdc++.h>
using namespace std;

class Location{
public:
    int latitute;
    int longitude;
    string pincode;
    string city;
};

class Rider{
public:
    int id;
    string name;
    Location location;
    string city;
};

class Driver{
public:
    enum class Status{
        AVAILABLE,
        BUSY,
        OFFLINE
    };
    int id;
    string name;
    Location location;
    string city;
    Status status;
};

class Ride{
public:
    enum class Status{
        ASSIGNED,
        STARTED,
        COMPLETED
    };
    int driverId;
    int riderId;
    Location start;
    Location end;
    double amount;
    Status status;
};

class MatchingStrategy{
    double dist(Location driver, Location rider){
        return sqrt(pow(abs(driver.latitute - rider.latitute), 2) + pow(abs(driver.longitude - rider.longitude), 2));
    }
public:
    virtual int getBestDriver(unordered_map<int, shared_ptr<Driver>>& drivers, Location riderLocation){
        int bestDriverId;
        double minDistance = 1e10;
        for(auto& [id, driver]: drivers){
            if( driver->status != Driver::Status::AVAILABLE) 
                continue;
            double curDistance = dist(driver->location, riderLocation);
            if(curDistance < minDistance){
                curDistance = minDistance;
                bestDriverId = id;
            }
        }

        return bestDriverId;
    }
};

// Singleton
class DriverService{
    unordered_map<int, shared_ptr<Driver>> drivers;
    shared_ptr<MatchingStrategy> matchingStrategy;
    DriverService(shared_ptr<MatchingStrategy> matchingStrategy): matchingStrategy(matchingStrategy) {}
public:
    static shared_ptr<DriverService> getInstance(){
        static shared_ptr<DriverService> instance  = shared_ptr<DriverService>(new DriverService(make_shared<MatchingStrategy>()));

        return instance;
    }

    void addDriver(shared_ptr<Driver> driver){
        drivers[driver->id] = driver;
    }

    int getBestDriver(Location riderLocation){
        int bestDriver = matchingStrategy->getBestDriver(drivers, riderLocation);
        drivers[bestDriver]->status = Driver::Status::BUSY;

        cout<<"Driver Id assigned: "<<bestDriver<<" Name: "<<drivers[bestDriver]->name<<endl;

        return bestDriver;
    }

    void updateDriverLocation(int driverId, Location loc){
        drivers[driverId]->location = loc; 
    }
};

class MatchingEngine;


class PaymentProcessor{
public:
    virtual bool process(double amount){
        return true;
    }
};

class PricingStrategy{
public:
    virtual double calculate(Location start, Location end){
        cout<<"Ride Fee calculated: "<<100<<endl;
        return 100;
    }
};

class Uber{
    shared_ptr<DriverService> driverService; 
    unordered_map<int, shared_ptr<Ride>> rides;
    shared_ptr<PaymentProcessor> paymentProcessor;
    shared_ptr<PricingStrategy> pricingStrategy;
    int rcounter=0;
public:
    Uber(shared_ptr<DriverService> driverService)
        : driverService(driverService), paymentProcessor(make_shared<PaymentProcessor>()), pricingStrategy(make_shared<PricingStrategy>()) {}

    int createRide(Rider rider, Location dropLocation){
        int driverId = driverService->getBestDriver(rider.location);
        auto ride = shared_ptr<Ride>(new Ride{driverId, rider.id, rider.location, dropLocation, pricingStrategy->calculate(rider.location, dropLocation), Ride::Status::STARTED});
        int rideId = ++rcounter;
        rides[rideId] = ride;
        
        return rideId;
    }

    void endRide(int rideId){
        if(paymentProcessor->process(rides[rideId]->amount)){
            rides[rideId]->status = Ride::Status::COMPLETED;
            driverService->updateDriverLocation(rides[rideId]->driverId, rides[rideId]->end);
        }

        throw logic_error("Payement Failed");
    }
};

int main(){

    auto location = Location{1, 1, "560103", "Bengaluru"};
    auto busStop = Location{-3, 1, "560105", "Bengaluru"};
    auto airport = Location{100, 100, "560999", "Bengaluru"};

    auto chetan = Rider{1, "chetan", location, "Bengaluru"};
    auto driver = shared_ptr<Driver>(new Driver{11, "driver", busStop, "Bengaluru"});

    auto driverService = DriverService::getInstance();
    driverService->addDriver(driver);

    auto rideSharingService = make_shared<Uber>(driverService);

    cout<<rideSharingService->createRide(chetan, airport);

    return 0;
}