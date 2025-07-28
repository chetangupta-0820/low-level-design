#include<bits/stdc++.h>
using namespace std;

class FifoSemaphore{
	mutex mtx;
	condition_variable cv;
	int count;
	unsigned int cur;
	queue<unsigned int> q; // wating queue for acquiring lock

public:
	FifoSemaphore(int count): count(count), cur(0) {}
	
	void acquire(){
		unique_lock<mutex> lock(mtx);
		
		unsigned int myTicket = cur++;
		q.push(myTicket);
		
		cv.wait(lock, [this, myTicket]{
			return count > 0 && q.front()== myTicket ; 
		});
		
		count--;
		q.pop(); // remove itself from queue
		cv.notify_all(); // to allow next waiting thread to enter critical section
	}
	
	void release(){
		unique_lock<mutex> lock(mtx);
		count++;
		cv.notify_all();
	}
};

void doWork(int i){
	cout<<"Thread " + to_string(i) + "  working...\n";
	this_thread::sleep_for(chrono::milliseconds(500)); 
}

void run(int id, FifoSemaphore &sem){
	sem.acquire();
	doWork(id);
	sem.release();
}

int32_t main() {
	FifoSemaphore sem(5);
	vector<thread> threads;
	for(int i=0;i<20;++i){
		threads.emplace_back(run, i, ref(sem));
	}
	
	for(auto &t:threads) t.join();

  return 0;
}