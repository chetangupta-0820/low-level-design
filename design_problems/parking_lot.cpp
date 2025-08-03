#include<bits/stdc++.h>
using namespace std;

enum class VehicleType{
    CAR = 4,
    BIKE = 2
};

enum class MotorType{
    FUEL,
    ELECTRIC
};

class Vehicle{
public:
    int number;
    VehicleType vehicleType;
    MotorType motorType;
    double baseFee;
    virtual ~Vehicle() = default;
    Vehicle(int number, VehicleType vehicleType, MotorType motorType, double baseFee)
         : number(number), vehicleType(vehicleType), motorType(motorType), baseFee(baseFee) {}
};

class Car: public Vehicle{
public:
    Car(int number, MotorType motorType): Vehicle(number, VehicleType::CAR, motorType, 100.0) {}
};

class Bike: public Vehicle{
public:
    Bike(int number, MotorType motorType): Vehicle(number, VehicleType::BIKE, motorType, 50.0) {}
};

enum class SpotType{
    REGULAR,
    ELECTRIC 
};

class ParkingSpot{
public:
    int capacity;
    SpotType type;
    bool isFree;
    int occupiedAt;
    double multiplier;
    virtual ~ParkingSpot() = default;
    ParkingSpot(int capacity, SpotType type, double multiplier)
        : capacity(capacity), type(type), isFree(true), multiplier(multiplier), occupiedAt(-1) {}

    virtual bool canPark(shared_ptr<Vehicle> vehicle) = 0;
};

class RegularSpot: public ParkingSpot{
public:
    RegularSpot(int capacity) : ParkingSpot(capacity, SpotType::REGULAR, 1.0){}

    bool canPark(shared_ptr<Vehicle> vehicle){
        return static_cast<int>(vehicle->vehicleType) <= capacity; 
    }
};

class ElectricSpot: public ParkingSpot{
public:
    ElectricSpot(int capacity) : ParkingSpot(capacity, SpotType::ELECTRIC, 2.0){}

    bool canPark(shared_ptr<Vehicle> vehicle){
        return (vehicle->motorType==MotorType::ELECTRIC) && static_cast<int>(vehicle->vehicleType) <= capacity ; 
    }
};

class ParkingFloor{
public:
    vector<vector<shared_ptr<ParkingSpot>>> spots;

    ParkingFloor(vector<vector<shared_ptr<ParkingSpot>>> parkingSpots) : spots(parkingSpots) {}
};

class PaymentStrategy{
public:
    virtual bool processPayment(double amount) = 0;
};

class CreditCard: public PaymentStrategy{
public:
    bool processPayment(double amount){
        return true;
    }
};

class ParkingFeeStrategy{
public:
    double getParkingFee(shared_ptr<Vehicle> vehicle, shared_ptr<ParkingSpot> spot, int durationInMins){
        return vehicle->baseFee * spot->multiplier;
    }
};


// singleton
class ParkingLot {
private:
    vector<shared_ptr<ParkingFloor>> floors;
    shared_ptr<PaymentStrategy> paymentStrategy;
    shared_ptr<ParkingFeeStrategy> parkingFeeStrategy;
    unordered_map<int, shared_ptr<ParkingSpot>> vehicleToSpotMap; // Track where each vehicle is parked

    double calculateFee(shared_ptr<Vehicle> vehicle, shared_ptr<ParkingSpot> spot){
        int curTime = static_cast<int>(time(0)/60); // Convert to minutes directly
        return parkingFeeStrategy->getParkingFee(vehicle, spot, curTime - spot->occupiedAt); 
    }

    ParkingLot(vector<shared_ptr<ParkingFloor>> floors)
        : floors(floors), parkingFeeStrategy(make_shared<ParkingFeeStrategy>()), 
          paymentStrategy(make_shared<CreditCard>()) {}

public:
    static shared_ptr<ParkingLot> getInstance(vector<shared_ptr<ParkingFloor>> floors){
        static shared_ptr<ParkingLot> instance = shared_ptr<ParkingLot>(new ParkingLot(floors));
        return instance;
    }

    shared_ptr<ParkingSpot> getFreeSpot(shared_ptr<Vehicle> vehicle){
        if (vehicle == nullptr) {
            throw invalid_argument("Invalid vehicle");
        }

        // Check if vehicle is already parked
        if (vehicleToSpotMap.find(vehicle->number) != vehicleToSpotMap.end()) {
            throw logic_error("Vehicle already parked");
        }

        for(auto floor: floors){
            for(auto row:floor->spots){
                for(auto spot: row){
                    if(spot->isFree && spot->canPark(vehicle)){
                        spot->isFree = false;
                        spot->occupiedAt = static_cast<int>(time(0)/60);
                        vehicleToSpotMap[vehicle->number] = spot;
                        return spot;
                    }
                }
            }
        }

        throw logic_error("No free spot available");
    }

    double checkout(shared_ptr<Vehicle> vehicle){
        if (vehicle == nullptr) {
            throw invalid_argument("Invalid vehicle");
        }

        auto it = vehicleToSpotMap.find(vehicle->number);
        if (it == vehicleToSpotMap.end()) {
            throw logic_error("Vehicle not found in parking lot");
        }

        auto spot = it->second;
        double fee = calculateFee(vehicle, spot);

        if(paymentStrategy->processPayment(fee)){
            spot->isFree = true;
            spot->occupiedAt = -1;
            vehicleToSpotMap.erase(it);
            return fee;
        }

        throw logic_error("Payment Failed");
    }

};

// factory
shared_ptr<Vehicle> createVehicle(int number, MotorType motorType, VehicleType vehicleType){
    switch(vehicleType){
        case VehicleType::CAR : return make_shared<Car>(number, motorType);
        case VehicleType::BIKE : return make_shared<Bike>(number, motorType);
        default: throw logic_error("Unsupported Vehicle");
    }
}

int main(){
    try {
        // Create vehicles with unique numbers
        auto car1 = createVehicle(123, MotorType::ELECTRIC, VehicleType::CAR);
        auto car2 = createVehicle(234, MotorType::ELECTRIC, VehicleType::CAR);
        auto car3 = createVehicle(345, MotorType::ELECTRIC, VehicleType::CAR);
        auto bike1 = createVehicle(456, MotorType::ELECTRIC, VehicleType::BIKE);

        // Create parking structure
        auto spots1 = vector<vector<shared_ptr<ParkingSpot>>>{
            vector<shared_ptr<ParkingSpot>>{
                make_shared<RegularSpot>(2), 
                make_shared<ElectricSpot>(4)
            }
        };

        auto spots2 = vector<vector<shared_ptr<ParkingSpot>>>{
            vector<shared_ptr<ParkingSpot>>{
                make_shared<RegularSpot>(2), 
                make_shared<ElectricSpot>(4)
            }
        };

        auto floors = vector<shared_ptr<ParkingFloor>>{
            make_shared<ParkingFloor>(spots1), 
            make_shared<ParkingFloor>(spots2)
        };

        auto lot = ParkingLot::getInstance(floors);

        // Park vehicles
        cout << "Parking vehicles...\n";
        auto spot1 = lot->getFreeSpot(car1);
        cout << "Car1 parked successfully\n";
        
        auto spot2 = lot->getFreeSpot(car2);
        cout << "Car2 parked successfully\n";
        
        auto spot3 = lot->getFreeSpot(bike1);
        cout << "Bike1 parked successfully\n";

        // Checkout vehicles
        cout << "\nChecking out vehicles...\n";
        double fee1 = lot->checkout(car1);
        cout << "Car1 checkout complete. Fee: $" << fee1 << endl;
        
        double fee2 = lot->checkout(car2);
        cout << "Car2 checkout complete. Fee: $" << fee2 << endl;
        
        double fee3 = lot->checkout(bike1);
        cout << "Bike1 checkout complete. Fee: $" << fee3 << endl;

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}