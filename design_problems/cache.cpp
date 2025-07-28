#include<bits/stdc++.h>
using namespace std;

template<typename K>
class EvictionPolicy{
public:
virtual ~EvictionPolicy() = default;
    virtual void keyAccessed(K key) = 0;
    virtual K evictKey() = 0;
};

template<typename K>
class LRUEvictionPolicy: public EvictionPolicy<K>{
    class Node{
    public:
        K key;
        Node* next;
        Node* prev;

        Node(K key, Node* next, Node* prev){
            this->key = key;
            this->next = next;
            this->prev = prev;
        }

        Node(K key){
            this->key = key;
            this->next = nullptr;
            this->prev = nullptr;
        }
    };

    Node* head;
    Node* tail;
    map<K, Node*> pos;

    void deleteNode(Node* node, bool free_memory = false){
        Node* nextNode = node->next;
        Node* prevNode = node->prev;

        prevNode->next = nextNode;
        nextNode->prev = prevNode;

        if(free_memory)
            delete node;
    }

    void moveToFront(Node* node){
        Node* firstNode = head->next;
        firstNode->prev = node;

        head->next = node;
        node->next = firstNode;

        node->prev = head;
    }

    mutex mtx;

public:
    LRUEvictionPolicy(){
        head = new Node(K{});
        tail = new Node(K{});

        head->next = tail;
        tail->prev = head;
    }

    void keyAccessed(K key){
        lock_guard<mutex> lock(mtx);
        Node* it;
        if(!pos.count(key))
            it = new Node(key);
        else{
            it = pos[key];
            deleteNode(it);
        }

        moveToFront(it);
        pos[key] = head->next;
    }

    K evictKey(){
        lock_guard<mutex> lock(mtx);
        auto lru = tail->prev;
        if(lru == head)
            throw logic_error("key not found");
        K key  = lru->key;
        deleteNode(lru, true);
        pos.erase(key);
        return key;
    }
};

template <typename K, typename V>
class CacheStorage{
public:
    virtual ~CacheStorage() = default;
    virtual void put(K key, V value) = 0;
    virtual V get(K key) = 0;
    virtual void remove(K key) = 0;
    virtual int getSize() = 0;
    virtual bool contains(K key) = 0;

};

template <typename K, typename V>
class InMemoryCache: public CacheStorage<K, V>{
    unordered_map<K, V> data;
public:
    void put(K key, V value){
        data[key] = value;
    } 

    V get(K key){
        if(data.count(key))
            return data[key];
        throw logic_error("Key not found");
    }

    void remove(K key){
        if(data.count(key))
            data.erase(key);
        else
            throw logic_error("Key not found");
    }
    
    int getSize(){
        return data.size();
    }

    bool contains(K key){
        return data.count(key);
    }
};


template<typename K, typename V>
class Cache{
    shared_ptr<CacheStorage<K,V>> cache;
    shared_ptr<EvictionPolicy<K>> evictionPolicy;
    int capacity;
    array<shared_mutex, 10> locks;
    
    shared_mutex& getLock(const K& key){
        int idx = hash<K>{}(key)%10;
        return locks[idx];
    }    

public:
    Cache( shared_ptr<CacheStorage<K,V>> cache, shared_ptr<EvictionPolicy<K>> evictionPolicy, int capacity = 10): cache(cache), evictionPolicy(evictionPolicy), capacity(capacity) {}

    void put(K key, V value){
        K evicted;
        {
            unique_lock<shared_mutex> lock(getLock(key));
            cache->put(key, value);
            evictionPolicy->keyAccessed(key);
            if(cache->getSize() > this->capacity){
                evicted = evictionPolicy->evictKey();
            }
        }
        if(evicted){
            unique_lock<shared_mutex> lock(getLock(evicted)); // get next lock
            cache->remove(evicted);
            cout<< evicted << " key evicted"<< endl;
        }
    }

    V get(K key){
        shared_lock<shared_mutex> lock(getLock(key));
        V value;
        if(cache->contains(key))
            value = cache->get(key);
        else 
            throw logic_error("key not found");

        evictionPolicy->keyAccessed(key);
        return value;
    }   

    void remove(K key){
        unique_lock<shared_mutex> lock(getLock(key));
        if(cache->contains(key))
            cache->remove(key);
    }

};

int main(){
    
    using K = int;
    using V = std::string;

    auto store  = std::make_shared<InMemoryCache<K,V>>();
    auto policy = std::make_shared<LRUEvictionPolicy<K>>();
    Cache<K,V>  cache(store, policy, /*capacity*/ 3);

    auto worker = [&](int id) {
        std::mt19937 rng(id * 12345);
        std::uniform_int_distribution<int> keyDist(1,6);
        for (int i = 0; i < 15; ++i) {
            int k = keyDist(rng);
            std::string v = "T" + std::to_string(id)+"_"+std::to_string(k);

            try { cache.put(k,v); }
            catch (const std::exception& e) {
                std::cerr << "[tid " << id << "] put("<<k<<") : "<<e.what()<<'\n';
            }

            try {
                auto got = cache.get(k);
                std::cout << "[tid "<<id<<"] get("<<k<<") = "<<got<<'\n';
            } catch (const std::exception& e) {
                std::cout << "[tid "<<id<<"] miss("<<k<<") : "<<e.what()<<'\n';
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    std::vector<std::thread> th;
    for (int i=1;i<=4;++i) th.emplace_back(worker,i);
    for (auto& t: th) t.join();

    std::cout << "\nFinal cache size = " << store->getSize() << '\n';
    for (int k=1;k<=6;++k)
        if (store->contains(k))
            std::cout << "  key "<<k<<" present\n";
        
    return 0;
}