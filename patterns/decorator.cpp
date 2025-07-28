#include<bits/stdc++.h>
using namespace std;

class Coffee{
public:
    virtual ~Coffee() = default;
    virtual int cost() = 0;
};

class Espresso:public Coffee{
public:
    int cost(){
        return 5;
    }
};

class Latte:public Coffee{
public:
    int cost(){
        return 10;
    }
};

class Decorator:public Coffee{
protected:
    shared_ptr<Coffee> wrapee;
public:
    Decorator(shared_ptr<Coffee> coffee) : wrapee(coffee) {}
};

class SugarDecorator:public Decorator{
public:
    SugarDecorator(shared_ptr<Coffee> coffee) : Decorator(coffee) {}
    int cost(){
        return 2 + wrapee->cost();
    }
};

class MilkDecorator:public Decorator{
public:
    MilkDecorator(shared_ptr<Coffee> coffee) : Decorator(coffee) {}
    int cost(){
        return 3 + wrapee->cost();
    }
};

int main(){
    shared_ptr<Coffee> coffee = make_shared<Latte>();
    coffee = make_shared<SugarDecorator>(coffee);
    coffee = make_shared<MilkDecorator>(coffee);

    cout<<coffee->cost()<<endl;

    return 0;
}