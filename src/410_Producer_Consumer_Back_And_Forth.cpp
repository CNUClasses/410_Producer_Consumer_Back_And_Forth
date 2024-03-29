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
	cout<<s<<endl;
}

void producer(int id, int numbcounts) {

	for (int i=0;i<numbcounts;i++){
		//produce one
		report("Producer "+ to_string(id)+", produced 1, gCount="+to_string(gCount));


		//wait until consumer consumes
	}

	//indicate we are done
}

void consumer(int id) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1));	//wait before aquiring lock below
	while (true){
		//wait until one produced

		//Own lck here
		if(gCount>=1){
			//consume it
			gCount--;

			//notify the producer
			report("       Consumer " + to_string(id)+" consumed 1, gCount="+to_string(gCount));
		}

		//break if all work done

	}
	report("    Consumer: " +to_string(id)+" exiting");
}

int main() {
	cout << "The initial value of gCount is " << gCount << endl; //

	thread t_producer(producer,1,1000);
	thread t_producer1(producer,2, 1000);
	cin.get();  //causes a delay
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
