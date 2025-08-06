#include<bits/stdc++.h>
using namespace std;

class Tweet{
public:
    int id;
    int userId;
    long long timestamp;
    string content;
};

int main(){
    auto t = shared_ptr<Tweet>(new Tweet{1, 123, time(0), "hello world"});
    cout<<t->id<<" "<<t->userId<<" "<<t->timestamp<<" "<<t->content<<endl;
    return 0;
}