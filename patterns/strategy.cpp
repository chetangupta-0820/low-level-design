#include<bits/stdc++.h>
using namespace std;

class DriveStrategy{
	
public:
	virtual ~DriveStrategy(){}
	virtual void execute() = 0;
};

class SpecialDrive:public DriveStrategy{
public:
	void execute(){}
};

// abstract class
class Vehicle{
	shared_ptr<DriveStrategy> driveStrategy;

public:
  Vehicle(shared_ptr<DriveStrategy> strategy) : driveStrategy(strategy) {}
	virtual ~Vehicle(){}
	virtual void update() = 0;	
	virtual void render() = 0;
	void drive(){
		driveStrategy->execute();
	}
  void setDriveStrategy(shared_ptr<DriveStrategy> strategy) {
      driveStrategy = strategy;
  }
}; 


class LuxuryVehicle:public Vehicle{
public:
	LuxuryVehicle(): Vehicle(make_shared<SpecialDrive>()) {}
};

int32_t main() {

    unique_ptr<Vehicle> lv = make_unique<LuxuryVehicle>();
    lv->drive();
    return 0;
}