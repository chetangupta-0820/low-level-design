#include<bits/stdc++.h>
using namespace std;

template<typename T, typename K>
inline bool isType(shared_ptr<K> k) {
    return typeid(T) == typeid(*k);
}

template<typename T, typename K>
inline bool isType(const K& k) {
    return typeid(T) == typeid(k);
}

enum class ItemType{
    COLD_DRINK,
    SNACKS,
    DRY_FRUIT
};

class Item{
public:
    ItemType type;
    string brand;
    int price;
    
    Item() = default;
    Item(string brand, int price):brand(brand), price(price){}
    virtual ~Item() = default;
    virtual string getType() = 0; 
};

class ColdDrink:public Item{
public:
    ColdDrink(string brand, int price): Item(brand, price) {
        this->type = ItemType::COLD_DRINK;
    }
};

class Snacks:public Item{
public:
    Snacks(string brand, int price): Item(brand, price) {
        this->type = ItemType::SNACKS;
    }
};

class ItemShelf{
public:
    int id;
    shared_ptr<Item> item;
    int count;

    ItemShelf(int id, shared_ptr<Item> item, int count): id(id), item(item), count(count){}
};

class VendingMachineContext;

class VendingMachineState{
public:
    virtual void next(shared_ptr<VendingMachineContext> ctx) = 0;
};

class IdleState:public VendingMachineState{
public:
    void next(shared_ptr<VendingMachineContext> ctx);
};

class SelectionState:public VendingMachineState{
public:
    void next(shared_ptr<VendingMachineContext> ctx);
};

class PendingState:public VendingMachineState{
public:
    void next(shared_ptr<VendingMachineContext> ctx);
};

class OutOfStockState:public VendingMachineState{
public:
    void next(shared_ptr<VendingMachineContext> ctx);
};

class DispenceState:public VendingMachineState{
public:
    void next(shared_ptr<VendingMachineContext> ctx);
};


class PaymentProcessor{
public:
    virtual bool processPayment(int amount) = 0;
};

class CreditCardPayment:public PaymentProcessor{
public:
    string number;
    
    CreditCardPayment(string number): number(number){}

    bool processPayment(int amount){
        cout<<"Deducting "<<amount<<"from credit card number "<<number<<endl;
    }
};

void IdleState::next(shared_ptr<VendingMachineContext> ctx){
    ctx->setState(make_shared<SelectionState>());
}

void SelectionState::next(shared_ptr<VendingMachineContext> ctx){
    if(!ctx->checkShelfInventory(ctx->selectedShelfId))
        ctx->setState(make_shared<OutOfStockState>());
    else
        ctx->setState(make_shared<PendingState>());
}

void PendingState::next(shared_ptr<VendingMachineContext> ctx){
    if(!ctx->paymentSucess)
        ctx->setState(make_shared<IdleState>());

    ctx->setState(make_shared<DispenceState>());
}

void DispenceState::next(shared_ptr<VendingMachineContext> ctx){
    ctx->setState(make_shared<IdleState>());
}

// void OutofStockState::next(shared_ptr<VendingMachineContext> ctx){
//     ctx->setState(make_shared<IdleState>());
// }

class VendingMachineContext:public enable_shared_from_this<VendingMachineContext>{
    int THRESHOLD_COUNT = 5;
public:
    shared_ptr<VendingMachineState> state;
    int selectedShelfId;
    int buyCount;
    bool paymentSucess;

    VendingMachineContext(shared_ptr<VendingMachineState> state): state(state){}

    void setState(shared_ptr<VendingMachineState> newState){
        state = newState;
    }

    shared_ptr<VendingMachineState> getState(){
        return state;
    }

    void next(){
        state->next(shared_from_this());
    }

    map<int, ItemShelf> items;
    shared_ptr<PaymentProcessor> paymentProcessor;

    void addItemShelf(int id, shared_ptr<Item> item, int count){
        if(isType<IdleState>(this->getState())){
            items[id] = ItemShelf(id, item, count);
        }
    }

    bool checkShelfInventory(int id){
        return items[id].count <= THRESHOLD_COUNT;            
    }

    bool checkAllInventory(){
        bool result = true;
        for(auto &[id, shelf]:items)
            result = result || checkShelfInventory(id);

        return result;
    }


    // start of the process
    void buyItem(int id, int count){
        if(isType<IdleState>(this->getState())){
            this->next(); // move to selection state
            selectItem(id, count);
        }else{
            cout<<"Cannot perform Operation"<<endl;
        }
    }

    void selectItem(int id, int count){
        if(isType<SelectionState>(this->getState())){
            this->selectedShelfId = id;
            this->buyCount = count;            
            this->next(); // move to pending/out_of_stock state
            if(isType<OutOfStockState>(this->getState())){
                this->next(); // move to idle state
                return;
            }

            processPayment();
        }else{
            cout<<"Cannot perform Operation"<<endl;
        }
    }

    void processPayment(){
        if(isType<PendingState>(this->getState())){
            if(paymentProcessor->processPayment(items[selectedShelfId].item->price * buyCount));
                paymentSucess = true;
            this->next(); // move to disapence/idle state
            dispenseItem();
        }else{
            cout<<"Cannot perform Operation"<<endl;
        }
    }

    void dispenseItem(){
        if(isType<DispenceState>(this->getState())){
            items[this->selectedShelfId].count -= this->buyCount;
            this->selectedShelfId = 0;
            this->buyCount = 0;
            this->paymentSucess = false;
            this->next(); // move to idle state
        }else{
            cout<<"Cannot perform Operation"<<endl;
        }
    }
};


int main(){

    return 0;
}