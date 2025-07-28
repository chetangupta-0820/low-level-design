#include<bits/stdc++.h>
using namespace std;

struct Callback{
    string message;
    long long exectime;
};

class comparator{
    bool operator()(const shared_ptr<Callback> a, const shared_ptr<Callback> b) const{
        return a->exectime < b->exectime ;
    }
};

class DeferredCallback{
    mutex mtx;
    condition_variable cv;
    priority_queue<shared_ptr<Callback>, vector<shared_ptr<Callback>>, comparator> pq;  
    thread worker;

    void start(){
        while(true){
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this]{
                return !pq.empty();
            });

            while(!pq.empty()){
                long long sleepFor  = pq.top()->exectime - time(0);
                
                if(sleepFor <=0)
                    break;
                
                cv.wait_for(lock, chrono::milliseconds(sleepFor));
            }

            auto callback = pq.top();
            pq.pop();
            cout<< callback->message << endl;
        }
    }
    
public:
    DeferredCallback(): worker(start, this) {}

    void registerCallback(shared_ptr<Callback> callback){
        unique_lock<mutex> lock(mtx);
        pq.push(callback);
        cv.notify_all();
    }

};


int main(){
    return 0;
}