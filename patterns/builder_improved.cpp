#include<bits/stdc++.h>
using namespace std;

class Car{
    // friend class Outer so that it can access it's private members 
    friend class Builder;
    string model_ = "base"; 
    string type_ = "normal";
    string colour_ = "black";
    int capacity_ = 5;
    bool sunroof_ = false;
    bool gps_ = false;

    Car() = default;

public:
    // public nested Builder class
    class Builder;

    void describe(){
        cout<<"Model: "<<this->model_<<endl;
        cout<<"Type: "<<this->type_<<endl;
        cout<<"Colour: "<<this->colour_<<endl;
        cout<<"Capacity: "<<this->capacity_<<endl;
        cout<<"Sunroof: "<<this->sunroof_<<endl;
        cout<<"GPS: "<<this->gps_<<endl;
    }
};

class Car::Builder{
    private:
        shared_ptr<Car> car;
    public:

        Builder(): car(shared_ptr<Car>(new Car)) {}

        // each setter method returns a reference to itself
        Builder& model(string model){
            car->model_ = model;
            return *this;
        }

        Builder& type(string type){
            car->type_ = type;
            return *this;
        }

        Builder& colour(string colour){
            car->colour_ = colour;
            return *this;
        }

        Builder& capacity(int capacity){
            car->capacity_ = capacity;
            return *this;
        }

        Builder& sunroof(bool sunroof){
            car->sunroof_ = sunroof;
            return *this;
        }

        Builder& gps(bool gps){
            car->gps_ = gps;
            return *this;
        }

        shared_ptr<Car> build(){
            return car;
        }
    };


int main(){

    auto sportsCar = Car::Builder()
                        .model("medium")
                        .type("sports")
                        .colour("red")
                        .gps(true)
                        .build();

    auto luxuryCar = Car::Builder()
                        .model("high")
                        .type("luxury")
                        .sunroof(true)
                        .gps(true)
                        .build();
    
    sportsCar->describe();
    cout<<endl;
    luxuryCar->describe();
}

