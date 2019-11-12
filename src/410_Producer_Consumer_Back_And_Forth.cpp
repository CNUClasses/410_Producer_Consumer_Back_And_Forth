//============================================================================
// Name        : 410_Producer_consumer
// Author      : 
// Version     :
// Copyright   : Steal this code!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>

using namespace std;

mutex m;				//mutual exclusion
condition_variable cv; 	//for signaling

bool bProducedOne = false;	
bool bConsumedOne = false;	
bool bDone = false;			//used by producer to indicate we are done
int gCount = 0;

void producer(int numbcounts) {
	cin.get();
	for (int j = 0; j < numbcounts; j++) {
		{
			unique_lock<mutex> lck(m);

			//produce something
			gCount++;
			cout << "Produced one, gCount=" << gCount << endl;
	
			bProducedOne = true;	//indicate one is ready
		}
		
		cv.notify_all();	//tell consumer to consume
//		cv.notify_one();	//tell consumer to consume

		
		//wait until consumer is done
		{
			unique_lock<mutex> lck(m);
			while(!bConsumedOne)
				cv.wait(lck);
			//reset
			bConsumedOne = false;
		}
	}

	//one last lock to tell everyone we are done
	{
		unique_lock<mutex> lck(m);
		bDone = true;
		cout << "Producer DONE!!" << endl;
	}
	
	//this will wake the consumer who sees the bDone==true and then leaves
	cv.notify_all();
}

void consumer(int id) {

	//first lets wait until producer is ready to go
	while (true){
		unique_lock<mutex> lck(m);

		while (!bProducedOne && !bDone)
			cv.wait(lck);

		if (bProducedOne){
			//consume
			gCount--;
			
			cout << "   Consumer: " << id<<" consumed one, gCount=" << gCount << endl;

			//reset (forget this and other consumers may decrement gCount)
			bProducedOne = false;		
			bConsumedOne = true;
			
			//and notify
			lck.unlock();
			cv.notify_all();
		}	

//		{
////			//what happens if we lock access?
//			unique_lock<mutex> lck(m);
			if(bDone == true)
				break;
//		}

	}
	unique_lock<mutex> lck(m);
	cout << "  Consumer: " << id << " exiting" << endl;
}

int main() {
	cout << "The initial value of gCount is " << gCount << endl; //

	thread t_producer(producer, 1000);
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	thread t_consumer3(consumer, 3);
	thread t_consumer4(consumer, 4);

	t_consumer1.join();
	t_consumer2.join();
	t_consumer3.join();
	t_consumer4.join();
	t_producer.join();

	cout << endl << "The final value of gCount is " << gCount << endl; //

	return 0;
}
