#include<bits/stdc++.h>
using namespace std;

class Car{

public:
    // public nested Builder class
    class Builder{
    private:
        // friend class Outer so that it can access it's private members 
        friend class Car;
        string model_ = "base"; 
        string type_ = "normal";
        string colour_ = "black";
        int capacity_ = 5;
        bool sunroof_ = false;
        bool gps_ = false;
    public:

        Builder() = default;


        // each setter method returns a reference to itself
        Builder& model(string model){
            this->model_ = model;
            return *this;
        }

        Builder& type(string type){
            this->type_ = type;
            return *this;
        }

        Builder& colour(string colour){
            this->colour_ = colour;
            return *this;
        }

        Builder& capacity(int capacity){
            this->capacity_ = capacity;
            return *this;
        }

        Builder& sunroof(bool sunroof){
            this->sunroof_ = sunroof;
            return *this;
        }

        Builder& gps(bool gps){
            this->gps_ = gps;
            return *this;
        }

        shared_ptr<Car> build(){
            return shared_ptr<Car>(new Car(*this));
        }
    };

    void describe(){
        cout<<"Model: "<<model<<endl;
        cout<<"Type: "<<type<<endl;
        cout<<"Colour: "<<colour<<endl;
        cout<<"Capacity: "<<capacity<<endl;
        cout<<"Sunroof: "<<sunroof<<endl;
        cout<<"GPS: "<<gps<<endl;
    }


private: 
    string model, type, colour;
    int capacity;
    bool sunroof, gps;
    

    // private constructor that can only be accessed by nested Builder
    Car(Builder& builder){
        this->model = builder.model_;
        this->type = builder.type_;
        this->colour = builder.colour_;
        this->capacity = builder.capacity_;
        this->sunroof = builder.sunroof_;
        this->gps = builder.gps_;
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

