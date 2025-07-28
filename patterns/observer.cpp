#include<bits/stdc++.h>
using namespace std;

class Observer{
public:
    string name;
    Observer(string name):name(name) {}
    ~Observer(){}
    virtual void update() =0;
};

class Observable{
    vector<shared_ptr<Observer>> observers;
    string name;

public:
    ~Observable(){}

    Observable() = default;

    Observable(string name): name(name) {}

    virtual void add(shared_ptr<Observer> observer){
        observers.push_back(observer);
    }

    virtual void remove(string name){
        for(auto observer:observers){
            if(observer->name == name){
                auto back = observers.back();
                swap(observer, back);
                observers.pop_back();
                break;
            }                
        }
    }

    virtual void notify_all(){
        for(auto observer:observers)
            observer->update();
    }      
};

class MobileSubscriber: public Observer{
public:
    MobileSubscriber(string name): Observer(name) {};
    void update(){
        cout<<name<<" MobileSubscriber notified"<<endl;
    }
};

class WebSubsriber: public Observer{
public:
    WebSubsriber(string name): Observer(name) {};
    void update(){
        cout<<name<<" WebSubsriber notified"<<endl;
    }
};

class YouTubeChannel: public Observable{
public:
    YouTubeChannel(string name): Observable(name) {}

    void upload(){
        cout<<"Uploading Video"<<endl;
        notify_all();
    }
};


int main(){

    auto channel = make_shared<YouTubeChannel>("My YT Channel");
    channel->add(make_shared<MobileSubscriber>("Chetan"));
    channel->add(make_shared<WebSubsriber>("Jayash"));
    channel->add(make_shared<MobileSubscriber>("Rushil"));

    channel->upload();

    channel->remove("Rushil");

    channel->upload();

    return 0;
}