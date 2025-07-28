#include<bits/stdc++.h>
using namespace std;

enum class ProductCategory{
    ELECTRONICS,
    FASHION
};


class Product{
    friend class Builder;
public:
    class Builder;

    ProductCategory category;
    int quantity = 5;
    double price = 11.0;
    int threshold = 100;
    string sku = "default";

    virtual ~Product() = default;
};

class Product::Builder{
    shared_ptr<Product> product;
public:
    Builder(): product(shared_ptr<Product>(new Product)) {}

    Builder& setQuantity(int quantity){
        product->quantity = quantity;
        return *this;
    }

    Builder& setCategory(ProductCategory category){
        product->category = category;
        return *this;
    }

    Builder& setPrice(double price){
        product->price = price;
        return *this;
    }   

    virtual shared_ptr<Product> build()=0;
};

class ElectronicProduct:public Product{
    string brand;
    int warranty;
    ElectronicProduct() = default;
public:
    class Builder;
};

class ElectronicProduct::Builder: public Product::Builder{
    shared_ptr<ElectronicProduct> eproduct;
public:
    Builder(): eproduct(shared_ptr<ElectronicProduct>(new ElectronicProduct)) {}
    string brand;
    int warranty;

    Builder& setBrand(string brand){
        this->brand = brand;
        return *this;
    }

    Builder& setWarranty(int warranty){
        this->warranty = warranty;
        return *this;
    }

    shared_ptr<Product> build(){
        return eproduct;
    }

};

// class FashionProduct:public Product{
//     string brand;
//     string size;
// public:
//     FashionProduct() : Product() {}
// };

class Warehouse{
    
public:
    int id;
    string name;
    string location;
    map<string, shared_ptr<Product>> products;

    void addProduct(shared_ptr<Product> product, int quantity){}
    void removeProduct(shared_ptr<Product> product, int quantity){}
    int getAvailableQuantity(string sku){
        if(products.count(sku)){
            return products[sku]->quantity;
        }
        return 0;
    }
};

// class ProductFactory{
//     public:
//     shared_ptr<Product> createProduct(ProductCategory category){
//         switch(category){
//             case ProductCategory::ELECTRONICS: return make_shared<ElectronicProduct>();
//             // case ProductCategory::FASHION: return make_shared<FashionProduct>();
//             default : throw logic_error("Unsupported category");
//         }
//     }
// };


class ReplenishStrategy{
public:
    virtual ~ReplenishStrategy() = default;
    virtual void replenish(shared_ptr<Product> product) = 0;
};

class JustInTimeReplenish: public ReplenishStrategy{
public: 
    JustInTimeReplenish(){}
    void replenish(shared_ptr<Product> product){}
};

class BulkReplenish: public ReplenishStrategy{
public: 
    BulkReplenish(){}
    void replenish(shared_ptr<Product> product){}
};

class Observer{
public:
    string name;
    void update(){
        cout<<"OBSERVER CALLED"<<endl;
    };
};

class InventoryManager{
public:
    vector<shared_ptr<Warehouse>> warehouses;
    // ProductFactory factory;
    shared_ptr<ReplenishStrategy> replenishStrategy;
    vector<shared_ptr<Observer>> observers;

    InventoryManager(InventoryManager&&) = delete;
    InventoryManager(const InventoryManager&) = delete;


    InventoryManager(shared_ptr<ReplenishStrategy> replenishStrategy): replenishStrategy(replenishStrategy){}

    static InventoryManager& getInstance(shared_ptr<ReplenishStrategy> replenishStrategy){
        static auto instance = shared_ptr<InventoryManager>(new InventoryManager(replenishStrategy));
        
        return *instance;
    }

    shared_ptr<Product> getProduct(string reqSku){
        for(auto warehouse: warehouses){
            for(auto [sku, product]:warehouse->products){
                if(sku == reqSku){
                    return product;
                }
            }
        }

        return nullptr;
    }

    void checkAndReplenish(string reqSku){
        for(auto warehouse: warehouses){
            for(auto [sku, product]:warehouse->products){
                if(sku == reqSku){
                    if(product->quantity < product->threshold){
                        notifyObservers();
                        replenishStrategy->replenish(product);
                    }
                }
            }
        }
    }

    void addObserver(shared_ptr<Observer> obs){
        observers.push_back(obs);
    }

    void removeObserver(string givenName){
        for(auto obs:observers){
            if(obs->name == givenName){
                swap(obs, observers.back());
                observers.pop_back();
                break;
            }
        }
    }

    void notifyObservers(){
        for(auto obs:observers){
            obs->update();
        }
    }
};


int main() {
    // 1) Build an ElectronicProduct
    auto eProd = ElectronicProduct::Builder()
                    .setBrand("AcmeCorp")
                    .setWarranty(24)
                    .setCategory(ProductCategory::ELECTRONICS)
                    .setQuantity(42)
                    .setPrice(799.99)
                    .build();

    // 2) Create a warehouse and add it to the inventory
    Warehouse wh;
    wh.id       = 1;
    wh.name     = "Central Depot";
    wh.location = "Bangalore";
    // directly put it into the public map:
    wh.products[eProd->sku] = eProd;

    // 3) Verify that getAvailableQuantity returns what we set
    cout << "SKU: " << eProd->sku
         << " has quantity: "
         << wh.getAvailableQuantity("default")
         << "\n";

    // 4) Now simulate a low‐stock trigger:
    //    drop the on‐hand quantity below threshold
    wh.products["default"]->quantity = 5;
    cout << "After selling 37 units, qty = "
         << wh.getAvailableQuantity("default")
         << "\n";

    // 5) Hook up InventoryManager (with a trivial strategy & observer)
    auto strat = make_shared<JustInTimeReplenish>();
    auto& mgr  = InventoryManager::getInstance(strat);

    // add our one warehouse to the manager by poking into its private vector:
    // (you could add an addWarehouse method if you like)
    // — for this test, we'll use a lambda hack
    struct Hack : InventoryManager { using InventoryManager::warehouses; };
    reinterpret_cast<Hack&>(mgr).warehouses.push_back(make_shared<Warehouse>(wh));

    mgr.addObserver(make_shared<Observer>());

    // finally invoke checkAndReplenish:
    mgr.checkAndReplenish("default");

    return 0;
}
