#include<bits/stdc++.h>
using namespace std;

class Singleton{
    
    // private constructor and destructor
    Singleton(){    
        cout<<"Singleton created"<<endl;
    }

    ~Singleton() = default;

    // delete copy and move connstructor
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;

public:
    // return reference 
    static Singleton& getInstance(){
        static Singleton instance = Singleton();

        return instance;
    }
};


int main(){
    cout<<"Program Started"<<endl;
    // use reference while calling get instance
    Singleton& instance = Singleton::getInstance();
    Singleton& instance2 = Singleton::getInstance();
}