#include<bits/stdc++.h>
using namespace std;

class Node{
public:
    ~Node() = default;
    virtual void execute(){};
};

class Leaf:public Node{

public:
    void execute(){
        cout<<"Leaf Node"<<endl;
    }
};


class Composite:public Node{
    vector<shared_ptr<Node>> children;
public:
    void add(shared_ptr<Node> node){
        children.push_back(node);
    }

    void execute(){
        cout<<"Composite Node"<<endl;
        for(auto child:children){
            child->execute();
        }
    }
};

template<typename T, typename K>
bool isType(const shared_ptr<K> &k){
    return typeid(T)==typeid(*k);
}

template<typename T, typename K>
bool isType(const K &k){
    return typeid(T)==typeid(k);
}

int main(){
    
    auto root = make_shared<Composite>();
    root->add(make_shared<Leaf>());
    auto comp = make_shared<Composite>();
    comp->add(make_shared<Leaf>());
    comp->add(make_shared<Leaf>());
    root->add(comp);

    root->execute();

    cout<<isType<Node>(*root);
    cout<<isType<Node>(*make_shared<Node>());
    cout<<isType<Node>(*make_shared<Leaf>());

    return 0;
}