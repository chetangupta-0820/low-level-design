#include<bits/stdc++.h>
using namespace std;

class Mediator;

class Collegue: public enable_shared_from_this<Collegue>{
public:
    string name;
    int id;
    shared_ptr<Mediator> mediator;

    Collegue(string name, int id): name(name), id(id) {}    
    virtual ~Collegue() = default;
    virtual void postEvent(double amount) = 0;
    virtual void receiveEvent(double amount) = 0;
    void setMediator(shared_ptr<Mediator> mediator){
        this->mediator = mediator;
    }
};

class Mediator: public enable_shared_from_this<Mediator>{
protected:
    vector<shared_ptr<Collegue>> collegues;
public:
    void addCollegue(shared_ptr<Collegue> collegue){
        collegue->setMediator(shared_from_this());
        collegues.push_back(collegue);
    }
    virtual void postEvent(shared_ptr<Collegue> collegue, double amount) = 0;
};

class Bidder: public Collegue{
public:
    Bidder(string name, int id): Collegue(name, id) {}
    void postEvent(double amount){
        mediator->postEvent(shared_from_this(), amount);
    }

    void receiveEvent(double amount){
        cout<<name<<" received a bid of "<<amount<<endl;
    }
};

class Auctioneer:public Mediator{
public:
    void postEvent(shared_ptr<Collegue> collegue, double amount){
        cout<<collegue->name <<" has posted a bid of "<<amount<<endl;

        for(auto bidder: collegues){
            if(bidder != collegue){
                bidder->receiveEvent(amount);
            }
        }
    }
};


int main(){
    auto auctioner = make_shared<Auctioneer>();
    auto chetan = make_shared<Bidder>("chetan", 1);
    auto deba = make_shared<Bidder>("deba", 2);
    auto svs = make_shared<Bidder>("harshith", 3);
    auctioner->addCollegue(chetan); 
    auctioner->addCollegue(deba); 
    auctioner->addCollegue(svs);

    chetan->postEvent(101);
    deba->postEvent(69);

    return 0;
}