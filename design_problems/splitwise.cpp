#include<bits/stdc++.h>
using namespace std;

class User{
    string name;
    string email;
    int id;

public:
    User() = default;
    User(string name, string email , int id): name(name), email(email), id(id){}
};

class Transaction{
public:
    int from;
    int to;
    double amount;

    Transaction(int from , int to, double amount): from(from), to(to), amount(amount) {}
};

class SplitStrategy{
public:
    virtual ~SplitStrategy() = default;
    virtual map<int, double> split(map<int, double> from) = 0;
};

class EqualSplit: public SplitStrategy{
public: 
    EqualSplit() = default;
    map<int, double> split(map<int, double> from){
        map<int, double> balances;
        if(from.empty())
            return balances;

        double total=0;
        int count=0;
        for(auto [userId, amt]: from)
            total += amt, count++;
            
        double amtPerUser = total/count;

        for(auto [userId, amt]:from){
            balances[userId] += amt;
            balances[userId] -= amtPerUser;
        }

        return balances;
    }
};

class Expense{
public:
    map<int, double> from;
    double total;
    shared_ptr<SplitStrategy> splitStrategy;
    map<int, double> shares;

    Expense() = default;

    void add(int userId, double contri){
        from[userId] += contri;
    }

    double getTotalContribution(int userId){
        if(from.count(userId))
            return from[userId];

        return 0;
    }

    void setSplitStrategy(shared_ptr<SplitStrategy> splitStrategy){
        this->splitStrategy = splitStrategy;
    }

    void split(){
        shares = splitStrategy->split(from);
    }
};

class SettlementStrategy{
public:
    virtual ~SettlementStrategy() = default;
    virtual vector<Transaction> getSettlements(map<int, double> &balances) = 0;
};

class BackTrackingSettlement: public SettlementStrategy{
    int solve(map<int, double>::iterator it, map<int, double> &balances, vector<Transaction> &optimal, vector<Transaction> &current){
        while (it != balances.end() && std::fabs(it->second) < 1e-9)
            ++it;
        
        if(it==balances.end()){
            if(optimal.empty() || (current.size()< optimal.size()))
                optimal = current;
            return current.size();
        }
        
        int n = balances.size();
        
        auto curBal = it->second;
        int minTxn = 1e5;
        for(auto nextIt = next(it); nextIt!=balances.end();++nextIt){
            auto nextBal = nextIt->second;
            if( (curBal*nextBal) <=0){
                nextIt->second += curBal;
                it->second = 0;
                double amt = std::fabs(curBal);
                int debtor   = curBal < 0 ?  it->first : nextIt->first;
                int creditor = curBal < 0 ? nextIt->first :  it->first;
                current.emplace_back(debtor, creditor, amt);  

                minTxn = min(minTxn, solve(next(it), balances, optimal, current));

                nextIt->second -= curBal;
                it->second = curBal;
                current.pop_back();
            }
        }

        return minTxn;
    }
public:
    BackTrackingSettlement() = default;
    vector<Transaction> getSettlements(map<int, double> &balances){
        auto it = balances.begin();
        vector<Transaction> optimal, current;
        solve(it, balances, optimal, current);

        return optimal;
    }

};

class Group{
    shared_mutex mtx;
public:
    map<int, double> balances;
    vector<Expense> expenses;
    vector<Transaction> settlements; 
    shared_ptr<SettlementStrategy> settlementStrategy;

    // write path
    void setSettlementStrategy(shared_ptr<SettlementStrategy> settlementStrategy){
        unique_lock<shared_mutex> lock(mtx); // write lock
        this->settlementStrategy = settlementStrategy;
    }

    // write path
    void addUser(int userId){
        unique_lock<shared_mutex> lock(mtx); // write lock
        balances[userId] = 0;
    }

    // write path 
    void addExpense(Expense exp){
        unique_lock<shared_mutex> lock(mtx); // write lock
        for (auto& [uid, _] : balances)
            exp.from.emplace(uid, exp.from[uid]);   // inserts 0 if uid absent

        exp.split();
        expenses.push_back(exp);

        for(auto [userId, amt]:exp.shares){
            balances[userId] += amt;
        }
    }

    // write path
    void getTransactions(){
        unique_lock<shared_mutex> lock(mtx); // write lock
        settlements = settlementStrategy->getSettlements(balances);
    }

    // read path 
    double getBalance(int userId){
        shared_lock<shared_mutex> lock(mtx); // read lock
        return balances[userId];
    }

    // read path -> use read lock
    void listTransactions();
};

// Singleton
class SplitWise{
    mutex umtx, gmtx;
    vector<shared_ptr<Group>> groups;
    map<int, User> users;

    int userIdCounter = 0;
    SplitWise(){}
public:
    // lazy loaded singleton instance 
    static shared_ptr<SplitWise> getInstance(){
        static shared_ptr<SplitWise> instance = shared_ptr<SplitWise>(new SplitWise());
        return instance;
    }

    // write path
    int addUser(string name, string email){
        unique_lock<mutex> lock(umtx);
        users[userIdCounter++] = User(name, email, userIdCounter);
        return userIdCounter;
    }

    // write path
    void addGroup(shared_ptr<Group> grp){
        grp->getTransactions();
        {
            unique_lock<mutex> lock(gmtx);
            groups.push_back(grp);
        }
    }
};

int main(){

    /* 1️⃣  Create the group and plug in a settlement strategy */
    Group trip;
    trip.setSettlementStrategy(std::make_shared<BackTrackingSettlement>());

    /* 2️⃣  Register users (ids 0,1,2) */
    for (int id : {0, 1, 2}) trip.addUser(id);

    /* 3️⃣  Define expenses – all of them EqualSplit */
    auto equal = std::make_shared<EqualSplit>();

    // Hotel (₹6 000) paid entirely by user-0
    Expense hotel;
    hotel.setSplitStrategy(equal);
    hotel.add(0, 6000);
    trip.addExpense(hotel);

    // Dinner (₹2 400) paid by user-1
    Expense dinner;
    dinner.setSplitStrategy(equal);
    dinner.add(1, 2400);
    trip.addExpense(dinner);

    // Fuel (₹1 200) paid by user-2
    Expense fuel;
    fuel.setSplitStrategy(equal);
    fuel.add(2, 1200);
    trip.addExpense(fuel);

    /* 4️⃣  Compute settlements */
    trip.getTransactions();

    /* 5️⃣  Print net balance per user */
    std::cout << "----- Net balances -----\n";
    for (const auto& [uid, bal] : trip.balances) {
        std::cout << "User " << uid
                  << (bal >= 0 ? " should receive ₹" : " owes ₹")
                  << std::fixed << std::setprecision(2) << std::fabs(bal) << '\n';
    }

    /* 6️⃣  Print the settlement plan */
    std::cout << "\n----- Suggested settlements (" 
              << trip.settlements.size() << ") -----\n";
    for (auto t : trip.settlements) {
        std::cout << "User " << t.from
                  << " pays User " << t.to
                  << " ₹" << std::fixed << std::setprecision(2)
                  << t.amount << '\n';
    }
    return 0;
}