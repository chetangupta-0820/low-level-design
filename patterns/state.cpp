#include<bits/stdc++.h>
using namespace std;


//forward declaration
class TrafficLight;

class TrafficState{
    
public:
    virtual ~TrafficState(){}
    virtual void show() = 0;
    virtual void next(shared_ptr<TrafficLight> light) = 0;
};

// forward declartion due to circular dependency
class YellowState: public TrafficState{
public:
    void show(){
        cout<<"YELLOW light - SLOW DOWN"<<endl;
    }

    void next(shared_ptr<TrafficLight> light) override;
};

class GreenState: public TrafficState{
public:
    void show(){
        cout<<"GREEN light - GO"<<endl;
    }

    void next(shared_ptr<TrafficLight> light) override;
    
};

class RedState: public TrafficState{
public:
    void show(){
        cout<<"RED light - STOP"<<endl;
    }

    void next(shared_ptr<TrafficLight> light) override;
};

// context class
class TrafficLight:public enable_shared_from_this<TrafficLight>{
    shared_ptr<TrafficState> state;
public:
    TrafficLight(shared_ptr<TrafficState> init) : state(init) {}

    void show(){
        state->show();
    }

    void setState(shared_ptr<TrafficState> newState ){
        state = newState;
    }

    void next(){
        state->next(shared_from_this());
    }

};


void YellowState::next(shared_ptr<TrafficLight> light){
    light->setState(make_shared<RedState>());
}

void GreenState::next(shared_ptr<TrafficLight> light){
    light->setState(make_shared<YellowState>());
}

void RedState::next(shared_ptr<TrafficLight> light){
    light->setState(make_shared<GreenState>());
}

int main(){

    auto light  = make_shared<TrafficLight>(make_shared<RedState>());

    for (int i = 0; i < 6; ++i) {
        light->show();
        light->next();
    }

    return 0;
}