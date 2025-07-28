#include<bits/stdc++.h>
using namespace std;

// abstract class
class Vehicle{

public:
	virtual ~Vehicle(){}
	virtual void update() = 0;	
	virtual void render() = 0;		
};

class Car:public Vehicle{

public:
	Car(){
		cout<<"Car created"<<endl;
	}
	void update(){}
	void render(){}
};

class Motorbike:public Vehicle{
	
public:
	Motorbike(){
		cout<<"MotorBike created"<<endl;
	}
	void update(){}
	void render(){}
};

enum class VehicleType{
	CAR,
	MOTORBIKE
};


// using smart pointers
shared_ptr<Vehicle> vehicleFactory(VehicleType type){
	switch(type){
		case(VehicleType::CAR) : return make_shared<Car>();
		case (VehicleType::MOTORBIKE) : return make_shared<Motorbike>();
		default: return nullptr;
	}
}

int32_t main() {

	shared_ptr<Vehicle>  myCar= vehicleFactory(VehicleType::CAR);     
	shared_ptr<Vehicle>  myBike= vehicleFactory(VehicleType::MOTORBIKE);
    
    return 0;
}
