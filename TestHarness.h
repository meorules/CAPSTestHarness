#pragma once
#include "MyThreadPool.h"
#include "barrier.hpp"
#include "TCPClient.h"
#include "RequestGenerator.h"
#include <future>
#include <chrono>
#include <vector>

using namespace std;

struct mathAverages {
	int noOfPostThreads;
	int noOfReadThreads;
	int noOfPostRequests;
	int noOfReadRequests;
	float time;
	float noOfPostRequestsPerSecond = noOfPostRequests/time;
	float noOfReadRequestsPerSecond = noOfReadRequests/time;
	float noOfPostRequestsPerThreadPerSecond = noOfPostRequestsPerSecond / noOfPostThreads;
	float noOfReadRequestsPerThreadPerSecond = noOfReadRequestsPerSecond / noOfReadThreads;
	float noOfRequestsPerThreadPerSecond = (noOfPostRequests + noOfReadRequests) / (noOfPostThreads+ noOfReadThreads) / time;

	mathAverages(int noOfPostThreads, int noOfReadThreads, int noOfPostRequests, int noOfReadRequests, float time) :noOfPostThreads(noOfPostThreads), noOfReadThreads(noOfReadThreads), noOfPostRequests(noOfPostRequests), noOfReadRequests(noOfReadRequests), time(time) {
	}
	~mathAverages(){}

	
};

ostream& operator<<(ostream& os, const mathAverages& mA)
{
	os << "Number of Post Requests: " << mA.noOfPostRequests << "\n";
	os << "Number of Read Requests: " << mA.noOfReadRequests << "\n";
	os << "Number of Post Requests Per Second: " << mA.noOfPostRequestsPerSecond << "\n";
	os << "Number of Read Requests Per Second: " << mA.noOfReadRequestsPerSecond << "\n";
	os << "Number of Post Requests Per Second Per Thread: " << mA.noOfPostRequestsPerThreadPerSecond << "\n";
	os << "Number of Read Requests Per Second Per Thread: " << mA.noOfReadRequestsPerThreadPerSecond << "\n";
	os << "Number of Requests Per Second Per Thread" << mA.noOfRequestsPerThreadPerSecond << "\n";

	return os;
}

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
	bool check;
	ThreadPool* threadPool;
	DataStructureAPI* dataStructure;
	std::chrono::nanoseconds finishTime;
	vector <future<vector<int>>> futureReadThreadReturns;
	vector <future<vector<int>>> futurePostThreadReturns;

	barrier* simpleBarrier;
	vector<int> readThread();
	vector<int> postThread();

	
	void storePostRequest(string postRequest, int id);

	bool checkPostRequest(string postRequest, string reply);

	bool checkReadRequest(string readRequest, string reply);

public:
	TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled,bool check=true);
	~TestHarness();

	void runTests();
	mathAverages* calculateAverages();


};

inline TestHarness::TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled,bool check) : serverIP(serverIP), numberOfPostThreads(numberOfPostThreads), numberOfReadThreads(numberOfReadThreads), totalNumberOfThreads(numberOfPostThreads + numberOfReadThreads), seconds(seconds), Throttled(Throttled),check(check) {
	threadPool = new ThreadPool(serverIP, totalNumberOfThreads);
	simpleBarrier = new barrier(totalNumberOfThreads);
	timeUp = false;
	dataStructure = new UnorderedMap();


}

TestHarness::~TestHarness() {
	delete threadPool;
	delete simpleBarrier;
}

void TestHarness::runTests() {
	for (int i = 0; i < numberOfPostThreads; i++) {
		//auto result = threadPool->QueueJob(postThread, simpleBarrier);
		//futureThreadReturns.emplace_back(std::move(result));
	}

	for (int i = 0; i < numberOfReadThreads; i++) {
		//auto result = threadPool->QueueJob(readThread, simpleBarrier);
		//futureThreadReturns.emplace_back(std::move(result));
	}

	while (simpleBarrier->getm_count() != simpleBarrier->getm_count_reset_value()) {
		//do nothing, just to pause execution until the barrier essentially gets hit
	}

	auto startTime = chrono::steady_clock::now();
	while (chrono::steady_clock::now() - startTime <= chrono::seconds(seconds)) {
		//do nothing, just to pause execution until the timer runs out essentially
	}
	timeUp = true;
	threadPool->Stop();

	finishTime = chrono::steady_clock::now() - startTime;

}

mathAverages* TestHarness::calculateAverages() {
	mathAverages* averageMath;
	int noOfPostRequests = 0;
	int noOfReadRequests = 0;

	std::chrono::duration<float> time;
	time = finishTime;
	for (int i = 0; i < futureReadThreadReturns.size(); i++) {
		for (int j = 0; i < futureReadThreadReturns.at(i).get().size(); j++) {
			noOfReadRequests += futureReadThreadReturns.at(i).get().at(j);
		}
	}
	for (int i = 0; i < futurePostThreadReturns.size(); i++) {
		for (int j = 0; i < futurePostThreadReturns.at(i).get().size(); j++) {
			noOfPostRequests += futurePostThreadReturns.at(i).get().at(j);
		}
	}
	averageMath = new mathAverages(numberOfPostThreads, numberOfReadThreads, noOfPostRequests, noOfReadRequests, time.count());
	return averageMath;
}



vector<int> TestHarness::postThread() {

	TCPClient client(serverIP, DEFAULT_PORT);

	client.OpenConnection();

	simpleBarrier->count_down_and_wait();

	RequestGenerator* gen = new RequestGenerator();
	std::string request;
	std::string reply;

	//send Post Requests
	while (!timeUp) {
		request= gen->generatePostRequest();
		reply = client.send(request);

		//Checking if the response is right
		if (check) {
			bool requestCheck = checkPostRequest(request, reply);
			//Need to implement 
		}

	}

	client.CloseConnection(); 



}

vector<int> TestHarness::readThread() {


	TCPClient client(serverIP, DEFAULT_PORT);

	client.OpenConnection();

	simpleBarrier->count_down_and_wait();


	//send read Requests
	while (!timeUp) {

	}
	std::string request;

	request = "";

	std::string reply = client.send(request);

	client.CloseConnection();

}

inline void TestHarness::storePostRequest(string postRequest, int id) {
	if (!postRequest.empty()) {
		string postTopic = postRequest.substr(5, postRequest.find("#"));
		string postMessage = postRequest.substr(postRequest.find("#"));
		dataStructure->PostFunction(postTopic, postMessage, id);
	}
}

inline bool TestHarness::checkPostRequest(string postRequest, string reply) {
	if (!postRequest.empty()) {
		string postTopic = postRequest.substr(5, postRequest.find("#"));
		string postMessage = postRequest.substr(postRequest.find("#"));
		string message = dataStructure->ReadFunction(postTopic, stoi(reply));
		if (postMessage == message) {
			return true;
		}
		else {
			return false;
		}
	}
}

inline bool TestHarness::checkReadRequest(string readRequest, string reply) {

}