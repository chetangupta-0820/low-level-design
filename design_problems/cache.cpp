#include<bits/stdc++.h>
using namespace std;

template<typename K>
class EvictionPolicy{
public:
    virtual ~EvictionPolicy() = default;
    virtual void keyAccessed(K key) = 0;
    virtual K evictKey() = 0;
    virtual void dropKey(K key) = 0;
};

template<typename K>
class LRUEvictionPolicy: public EvictionPolicy<K>{
    class Node{
    public:
        K key;
        shared_ptr<Node> next, prev;

        Node(K key, shared_ptr<Node> next = nullptr, shared_ptr<Node> prev = nullptr)
            : key(key), next(next), prev(prev) {}
    };

    shared_ptr<Node> head, tail;
    unordered_map<K, shared_ptr<Node>> pos;
    mutex mtx;

    void deleteNode(shared_ptr<Node> node){
        auto nextNode = node->next;
        auto prevNode = node->prev;

        nextNode->prev = prevNode;
        prevNode->next = nextNode;

        node->next = node->prev = nullptr;
    }

    void moveToFront(shared_ptr<Node> node){
        auto firstNode = head->next;

        node->next = firstNode;
        node->prev = head;

        head->next = node;
        firstNode->prev = node;
    }

public:
    LRUEvictionPolicy(){
        head = make_shared<Node>(K{});
        tail = make_shared<Node>(K{});

        head->next = tail;
        tail->prev = head;
    }

    void keyAccessed(K key){
        unique_lock<mutex> lock(mtx);
        shared_ptr<Node> it;
        if(!pos.count(key)){
            it = make_shared<Node>(key); 
        }
        else{
            it = pos[key];
            deleteNode(it);
        }

        moveToFront(it);
        pos[key] = head->next;
    }

    K evictKey(){
        unique_lock<mutex> lock(mtx);
        shared_ptr<Node> lru = tail->prev;

        if(lru == head)
            throw logic_error("No keys to evict");

        deleteNode(lru);
        pos.erase(lru->key);

        return lru->key;
    }

    void dropKey(K key){
        unique_lock<mutex> lock(mtx);
        if(pos.count(key)){
            auto it = pos[key];
            deletNode(it);
            pos.erase(it->key);
        }
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
    hash<K> hashfn;
    
    shared_mutex& getLock(const K& key){
        int idx = hashfn(key)%10;
        return locks[idx];
    }    

public:
    Cache( shared_ptr<CacheStorage<K,V>> cache, shared_ptr<EvictionPolicy<K>> evictionPolicy, int capacity = 10): cache(cache), evictionPolicy(evictionPolicy), capacity(capacity) {}

    void put(K key, V value){
        optional<K> evicted;
        {
            unique_lock<shared_mutex> lock(getLock(key));
            cache->put(key, value);
            evictionPolicy->keyAccessed(key);
            if(cache->getSize() > this->capacity){
                evicted = evictionPolicy->evictKey();
            }
        }

        if(evicted.has_value()){
            this->remove(evicted.value());
            // cout<< *evicted << " key evicted"<< endl;
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
        if(cache->contains(key)){
            cache->remove(key);
            evictionPolicy->dropKey(key);
        }
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