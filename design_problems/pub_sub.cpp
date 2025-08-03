#include<bits/stdc++.h>
using namespace std;

class Message{
public:
    string key;
    string content;
    int timestamp;
};

class Topic{
    mutex mtx;
public: 
    vector<Message> data;

    void push(Message msg){
        unique_lock<mutex> lock(mtx);
        data.push_back(msg);
    }

    size_t size(){
        unique_lock<mutex> lock(mtx);
        return data.size();
    }

};

class Subscriber{
public:
    virtual ~Subscriber() = default;
    virtual void onMessage(Message msg) = 0;
};

class DefaultSubscriber: public Subscriber{
    void onMessage(Message msg){
        cout<<"Printing Message "<< msg.key<<" "<<msg.content<<endl;
    }
};

class Publisher{
public:
    virtual ~Publisher() = default;
    virtual void publish(string topicId, Message msg) = 0;
};

class TopicSubscriber{
    shared_ptr<Subscriber> subscriber;
    shared_ptr<Topic> topic;
    int offset = 0;
    mutex mtx;
public:
    condition_variable cv;
    bool stop = false;

    TopicSubscriber(shared_ptr<Subscriber> subscriber, shared_ptr<Topic> topic): subscriber(subscriber), topic(topic) {}

    void run(){
        while(!stop){
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this]{
                return stop || offset < topic->size();
            });
            if(stop)
                break;
            auto& msg = topic->data[offset++];
            lock.unlock();
            subscriber->onMessage(msg);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }   
};

class KafkaController{
    unordered_map<string, shared_ptr<Topic>> topics;
    unordered_map<string,vector<shared_ptr<TopicSubscriber>>> consumers;
    vector<thread> workers;
    mutex gmtx;
public:
    ~KafkaController(){ 
        for( auto& [_, v]: consumers){
            for(auto& ts:v)
                ts->stop = true;
        }
        for (auto& t : workers){
            if (t.joinable())
                t.join();
        }  
    }

    void createTopic(string topicId){
        unique_lock<mutex> glock(gmtx);
        if(!topics.count(topicId))
            topics[topicId] = make_shared<Topic>();
    }

    void publish(shared_ptr<Publisher> publisher, string topicId, Message msg){
        if(!topics.count(topicId))
            throw logic_error("Invalid Topic Id");

        auto topic = topics[topicId];
        topic->push(msg);

        for(auto& consumer: consumers[topicId]){
            consumer->cv.notify_one();
        }
    }

    void subscribe(shared_ptr<Subscriber> subscriber, string topicId){
        if(!topics.count(topicId))
            throw logic_error("Invalid Topic Id");

        shared_ptr<Topic> topic = topics[topicId];
        shared_ptr<TopicSubscriber> consumer = make_shared<TopicSubscriber>(subscriber, topic);
        {
            unique_lock<mutex> glock(gmtx);
            consumers[topicId].push_back(consumer);
            workers.emplace_back([consumer]{
                consumer->run();
            });
        }
    }
};

class DefaultPublisher: public Publisher, public enable_shared_from_this<DefaultPublisher>{
    shared_ptr<KafkaController> controller;
public:
    DefaultPublisher(shared_ptr<KafkaController> controller): controller(controller) {}

    void publish(string topicId, Message msg){
        controller->publish(shared_from_this(), topicId, msg);
    }
};


int main(){

    auto broker = make_shared<KafkaController>();

    // create two topics
    broker->createTopic("orders");
    broker->createTopic("payments");

    // make two subscribers
    auto s1 = make_shared<DefaultSubscriber>();
    auto s2 = make_shared<DefaultSubscriber>();

    broker->subscribe(s1, "orders");    // s1 listens to both
    broker->subscribe(s1, "payments");
    broker->subscribe(s2, "payments");  // s2 listens only to payments

    // two publishers share the same broker
    auto p1 = make_shared<DefaultPublisher>(broker);
    auto p2 = make_shared<DefaultPublisher>(broker);

    /* ---------- publish some messages ---------- */
    using clk = chrono::system_clock;
    Message m1{ "o-1", "new order 1", static_cast<int>(clk::to_time_t(clk::now())) };
    Message m2{ "o-2", "new order 2", static_cast<int>(clk::to_time_t(clk::now())) };
    Message m3{ "p-1", "payment received", static_cast<int>(clk::to_time_t(clk::now())) };

    p1->publish("orders",   m1);
    p1->publish("orders",   m2);
    p2->publish("payments", m3);

    /* ---------- allow workers to consume ---------- */
    this_thread::sleep_for(3s);

    // more traffic
    Message m4{ "p-2", "refund issued", static_cast<int>(clk::to_time_t(clk::now())) };
    p2->publish("payments", m4);

    return 0;
}