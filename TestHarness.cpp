#include "TestHarness.h"


TestHarness::TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled, bool check) : serverIP(serverIP), numberOfPostThreads(numberOfPostThreads), numberOfReadThreads(numberOfReadThreads), totalNumberOfThreads(numberOfPostThreads + numberOfReadThreads), seconds(seconds), Throttled(Throttled), check(check),finishTime(0) {
	//threadPool = new ThreadPool(serverIP, totalNumberOfThreads);
	simpleBarrier = new barrier(totalNumberOfThreads);
	timeUp = false;
	dataStructure = new UnorderedMap();

}

TestHarness::~TestHarness() {
	delete simpleBarrier;
}

void myTest(TestHarness* obj, bool read = true)
{
	if (read)
		obj->readThread();
	else
		obj->postThread();

}

void TestHarness::runTests() {
	std::vector<std::thread> threads;
	for (int i = 0; i < numberOfPostThreads; i++) {
		threads.emplace_back(myTest,this,false);
		//futurePostThreadReturns.emplace_back(std::move(result));
	}

	for (int i = 0; i < numberOfReadThreads; i++) {
		threads.emplace_back(myTest, this,true);
		//futureReadThreadReturns.emplace_back(std::move(result));
	}

	while (simpleBarrier->getm_count() != simpleBarrier->getm_count_reset_value()) {
		//do nothing, just to pause execution until the barrier essentially gets hit
	}

	auto startTime = chrono::steady_clock::now();
	while (chrono::steady_clock::now() - startTime <= chrono::seconds(seconds)) {
		//do nothing, just to pause execution until the timer runs out essentially
	}
	timeUp = true;
	//threadPool->Stop();

	finishTime = chrono::steady_clock::now() - startTime;

}

mathAverages* TestHarness::calculateAverages() {
	mathAverages* averageMath;
	int noOfPostRequests = 0;
	int noOfReadRequests = 0;
	int noOfCorrectReadRequests = 0;
	int noOfCorrectWriteRequests = 0;


	std::chrono::duration<float> time;
	time = finishTime;
	for (int i = 0; i < futureReadThreadReturns.size(); i++) {
		noOfCorrectReadRequests += futureReadThreadReturns.at(i).get().at(futureReadThreadReturns.at(i).get().size() - 1);
		for (int j = 0; i < futureReadThreadReturns.at(i).get().size() - 2; j++) {
			noOfReadRequests += futureReadThreadReturns.at(i).get().at(j);
		}
	}
	for (int i = 0; i < futurePostThreadReturns.size(); i++) {
		noOfCorrectWriteRequests += futurePostThreadReturns.at(i).get().at(futurePostThreadReturns.at(i).get().size() - 1);
		for (int j = 0; i < futurePostThreadReturns.at(i).get().size() - 2; j++) {
			noOfPostRequests += futurePostThreadReturns.at(i).get().at(j);
		}
	}
	averageMath = new mathAverages(numberOfPostThreads, numberOfReadThreads, noOfPostRequests, noOfReadRequests, time.count(), noOfCorrectWriteRequests, noOfCorrectReadRequests);
	return averageMath;
}



vector<int> TestHarness::postThread() {

	TCPClient client(serverIP, DEFAULT_PORT);
	vector<int> noOfRequests;
	int noOfCorrectRequests = 0;

	client.OpenConnection();

	simpleBarrier->count_down_and_wait();

	RequestGenerator* gen = new RequestGenerator();
	std::string request;
	std::string reply;
	auto startTime = chrono::steady_clock::now();
	int currentNoOfRequests = 0;
	//send Post Requests
	while (!timeUp) {
		request = gen->generatePostRequest();
		reply = client.send(request);
		currentNoOfRequests++;

		//Checking if the response is right
		if (check) {
			bool requestCheck = checkPostRequest(request, reply);
			noOfCorrectRequests++;
		}

		if (chrono::steady_clock::now() - startTime >= chrono::seconds(1)) {
			noOfRequests.push_back(currentNoOfRequests);
			currentNoOfRequests = 0;
		}
	}
	noOfRequests.push_back(noOfCorrectRequests);


	simpleBarrier->count_down_and_wait();

	client.CloseConnection();


	return noOfRequests;

}

vector<int> TestHarness::readThread() {
	TCPClient client(serverIP, DEFAULT_PORT);
	vector<int> noOfRequests;
	int noOfCorrectRequests = 0;
	std::string request;
	std::string reply;
	RequestGenerator* gen = new RequestGenerator();
	int currentNoOfRequests = 0;

	client.OpenConnection();

	simpleBarrier->count_down_and_wait();

	auto startTime = chrono::steady_clock::now();
	//send Post Requests
	while (!timeUp) {
		//request = gen->generateRandomReadRequest();
		request = gen->generateReadRequest(dataStructure);
		reply = client.send(request);
		currentNoOfRequests++;

		//Checking if the response is right
		if (check) {
			bool requestCheck = checkReadRequest(request, reply);
			noOfCorrectRequests++;
		}

		if (chrono::steady_clock::now() - startTime >= chrono::seconds(1)) {
			noOfRequests.push_back(currentNoOfRequests);
			currentNoOfRequests = 0;
		}
	}
	noOfRequests.push_back(noOfCorrectRequests);

	simpleBarrier->count_down_and_wait();

	client.CloseConnection();

	return noOfRequests;
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
	if (!readRequest.empty()) {
		string postTopic = readRequest.substr(5, readRequest.find("#"));
		string postID = readRequest.substr(readRequest.find("#"));
		string message = dataStructure->ReadFunction(postTopic, stoi(postID));
		if (reply == message) {
			return true;
		}
		else {
			return false;
		}
	}
}