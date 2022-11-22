#include <iostream>
#include <future>
#include <chrono>
#include "TestHarness.h"

#define DEFAULT_PORT 12345

using namespace std;

/*TODO
*FIX IT
*/




/* @brief Main function used to run the test harness
*
* @param argc for count of arguements, and argv for the array of the various parameters
* 
*/
int main(int argc, char** argv)
{
	// Validate the parameters
	if (argc != 6) {
		printf("Parameters must be: \n Server IP \n Number of POST threads \n Number of READ threads \nTime Duration (in seconds) \n Throttled - 0(No) and 1(yes) ");
		return 1;
	}
	//Randomising the seed
	srand(time(NULL));

	//Collecting arguements from CLI
	char* serverIP = argv[1];
	int numberOfPostThreads = atoi(argv[2]);
	int numberOfReadThreads = atoi(argv[3]);
	int totalNumberOfThreads = numberOfPostThreads + numberOfReadThreads;
	int seconds = atoi(argv[4]);
	bool Throttled = argv[5];
	bool check = true;

	TestHarness* testHarness = new TestHarness(serverIP, numberOfPostThreads, numberOfReadThreads, seconds, Throttled,check);
	testHarness->runTests();
	mathAverages* mathAverage = testHarness->calculateAverages();
	std::cout << mathAverage;

	return 0;
}
