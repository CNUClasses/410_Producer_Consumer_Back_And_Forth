#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>

using namespace std;

mutex m;
mutex m_cout;
condition_variable cv; 	//for signaling
const int MAX_TO_PRODUCE_BEFORE_WAIT=5;
int numProducers=0;
int gCount = 0;

void report(string s){
	lock_guard<mutex> lck(m_cout);
	cout<<s<<endl;
}

void producer(int id, int numbcounts) {
	{
		unique_lock<mutex> lck(m);
		numProducers++;
	}

	for (int i=0;i<numbcounts;i++){
		//produce one
		{
			unique_lock<mutex> lck(m);
			gCount++;
			report("Producer "+ to_string(id)+", produced 1, gCount="+to_string(gCount));
		}
		cv.notify_all();	//notify consumer


		//wait until consumer consumes
		unique_lock<mutex> lck(m);
		while(gCount >= MAX_TO_PRODUCE_BEFORE_WAIT-1)
			cv.wait(lck);
	}

	{
		unique_lock<mutex> lck(m);
		numProducers--;
	}
	cv.notify_all();
}

void consumer(int id) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));	//wait before aquiring lock below
	while (true){
		//wait until one produced
		unique_lock<mutex> lck(m);
		while(!(gCount>=1) && (numProducers>0))	//while nothing produced and not done then wait
			cv.wait(lck);

		//I own lck here
		if(gCount>=1){
			//consume it
			gCount--;

			//notify the producer
			lck.unlock();
			report("       Consumer " + to_string(id)+" consumed 1, gCount="+to_string(gCount));
			cv.notify_all();
		}
		else if(numProducers==0 && gCount==0){
			break;
		}
	}
	report("    Consumer: " +to_string(id)+" exiting");
}

int main() {
	cout << "The initial value of gCount is " << gCount << endl; //

	thread t_producer(producer,1,1000);
	thread t_producer1(producer,2, 1000);
	cin.get();
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	thread t_consumer3(consumer, 3);
	thread t_consumer4(consumer, 4);

	t_consumer1.join();
	t_consumer2.join();
	t_consumer3.join();
	t_consumer4.join();
	t_producer.join();
	t_producer1.join();

	cout << endl << "The final value of gCount is " << gCount << endl; //

	return 0;
}
