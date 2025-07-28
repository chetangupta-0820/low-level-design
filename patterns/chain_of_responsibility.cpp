#include<bits/stdc++.h>
using namespace std;


class Handler{
    shared_ptr<Handler> nextHandler;
public:
    ~Handler() = default;

    virtual void setNext(shared_ptr<Handler> handler){
        nextHandler = handler;
    }

    virtual void handle(int days){
        if(nextHandler){
            cout<<" Forwarding to next handler..."<<endl;
            nextHandler->handle(days);
        }
        else
            cout<<"Request Denied"<<endl;
    }
};

class Director: public Handler{
public:
    Director() = default;

    void handle(int days){
        if(days<=14){
            cout<<"Director Approved"<<endl;
            return;
        }
        else{
            Handler::handle(days);
        }
    }
};


class Manager: public Handler{
public:
    Manager(){
        setNext(make_shared<Director>());
    }
    void handle(int days){
        if(days<=7){
            cout<<"Manager Approved"<<endl;
            return;
        }
        else{
            Handler::handle(days);
        }
    }
};

class Superviser: public Handler{
public:
    Superviser(){
        setNext(make_shared<Manager>());
    }
    void handle(int days){
        if(days<=3){
            cout<<"Superviser Approved"<<endl;
            return;
        }
        else{
            Handler::handle(days);
        }
    }
};

int main(){

    shared_ptr<Handler> superviser = make_shared<Superviser>();

    superviser->handle(2);
    superviser->handle(5);
    superviser->handle(11);
    superviser->handle(15);

    return 0;
}