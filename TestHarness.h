#pragma once
//#include "MyThreadPool.h"
#include "barrier.hpp"
#include "TCPClient.h"
#include "RequestGenerator.h"
#include "mathAverages.h"
#include <future>
#include <chrono>
#include <vector>


#define DEFAULT_PORT 12345
#define CHECK


using namespace std;

//NO THREAD POOLING

class TestHarness
{
private:
	char* serverIP;
	int numberOfPostThreads;
	int numberOfReadThreads;
	int totalNumberOfThreads;
	int seconds;
	bool Throttled;
	bool timeUp;
	//bool check;
	//ThreadPool* threadPool;
	DataStructureAPI* dataStructure;
	std::chrono::nanoseconds finishTime;
	vector <future<vector<int>>> futureReadThreadReturns;
	vector <future<vector<int>>> futurePostThreadReturns;

	vector <vector<int>> readThreadReturns;
	vector <vector<int>> postThreadReturns;

	barrier* simpleBarrier;
	

	
	void storePostRequest(string postRequest, int id);

	bool checkPostRequest(string postRequest, string reply);

	bool checkReadRequest(string readRequest, string reply);

public:
	//TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled,bool check=true);
	TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled);

	~TestHarness();

	void runTests();
	mathAverages* calculateAverages();

	vector<int> readThread();
	vector<int> postThread();


};
