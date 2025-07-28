#include<bits/stdc++.h>
using namespace std;

class TypeA;
class TypeB;

class Visitor{
public:
    virtual ~Visitor() = default;
    virtual void visit(shared_ptr<TypeA> element) = 0;
    virtual void visit(shared_ptr<TypeB> element) = 0;
};

class Element{
public:
    virtual void accept(shared_ptr<Visitor> visitor) = 0;
};

class TypeA: public Element, public enable_shared_from_this<TypeA>{
public:
    void accept(shared_ptr<Visitor> visitor){
        visitor->visit(shared_from_this());
    }
};

class TypeB: public Element, public enable_shared_from_this<TypeB>{
public:
    void accept(shared_ptr<Visitor> visitor){
        visitor->visit(shared_from_this());
    }
};

class OperationA: public Visitor{
public:
    void visit(shared_ptr<TypeA> element){
        cout<<"Performing operation A on Type A"<<endl;
    }
    void visit(shared_ptr<TypeB> element){
        cout<<"Performing operation A on Type B"<<endl;
    }
};

class OperationB: public Visitor{
    void visit(shared_ptr<TypeA> element){
        cout<<"Performing operation B on Type A"<<endl;
    }

    void visit(shared_ptr<TypeB> element){
        cout<<"Performing operation B on Type B"<<endl;
    }
};


int main(){
    auto opA = make_shared<OperationA>();
    auto opB = make_shared<OperationB>();
    auto tpA = make_shared<TypeA>();
    auto tpB = make_shared<TypeB>();

    tpA->accept(opA);
    tpA->accept(opB);
    tpB->accept(opA);
    tpB->accept(opB);    

    return 0;
}