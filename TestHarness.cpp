#include "TestHarness.h"


//TestHarness::TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled, bool check) : serverIP(serverIP), numberOfPostThreads(numberOfPostThreads), numberOfReadThreads(numberOfReadThreads), totalNumberOfThreads(numberOfPostThreads + numberOfReadThreads), seconds(seconds), Throttled(Throttled), check(check),finishTime(0) {
//	//threadPool = new ThreadPool(serverIP, totalNumberOfThreads);
//	simpleBarrier = new barrier(totalNumberOfThreads);
//	timeUp = false;
//	dataStructure = new UnorderedMap();
//
//}

TestHarness::TestHarness(char* serverIP, int numberOfPostThreads, int numberOfReadThreads, int seconds, bool Throttled) : serverIP(serverIP), numberOfPostThreads(numberOfPostThreads), numberOfReadThreads(numberOfReadThreads), totalNumberOfThreads(numberOfPostThreads + numberOfReadThreads), seconds(seconds), Throttled(Throttled), finishTime(0) {
	simpleBarrier = new barrier(totalNumberOfThreads);
	timeUp = false;
	dataStructure = new UnorderedMap();
}

TestHarness::~TestHarness() {
	delete simpleBarrier;
}

vector<int> myTest(TestHarness* obj, bool read = true)
{
	if (read)
		return obj->readThread();
	else
		return obj->postThread();

}

void TestHarness::runTests() {
	//std::vector<std::thread> threads;
	for (int i = 0; i < numberOfPostThreads; i++) {
		futurePostThreadReturns.push_back(async(myTest, this, false));
	}

	for (int i = 0; i < numberOfReadThreads; i++) {
		futureReadThreadReturns.push_back(async(myTest, this,true));
	}

	while (simpleBarrier->getm_count() != simpleBarrier->getm_count_reset_value()) {
		//do nothing, just to pause execution until the barrier essentially gets hit
	}

	auto startTime = chrono::steady_clock::now();
	while (chrono::steady_clock::now() - startTime <= chrono::seconds(seconds)) {
		//do nothing, just to pause execution until the timer runs out essentially
	}
	timeUp = true;
	/*for (int i = 0; i < threads.size(); i++) {
		threads.at(i).join();
	}*/
	//threadPool->Stop();


	for (int i = 0; i < numberOfPostThreads; i++) {
		postThreadReturns.push_back(futurePostThreadReturns.at(i).get());
	}

	for (int i = 0; i < numberOfReadThreads; i++) {
		readThreadReturns.push_back(futureReadThreadReturns.at(i).get());
	}

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

	
	for (int i = 0; i < readThreadReturns.size(); i++) {
#ifdef CHECK
		noOfCorrectReadRequests += readThreadReturns.at(i).at(readThreadReturns.at(i).size() - 1);
		for (int j = 0; j < readThreadReturns.at(i).size() - 1; j++) {
			noOfReadRequests += readThreadReturns.at(i).at(j);
		}
#else 
		for (int j = 0; j < readThreadReturns.at(i).size(); j++) {
			noOfReadRequests += readThreadReturns.at(i).at(j);
		}
#endif
	}
	
	for (int i = 0; i < postThreadReturns.size(); i++) {
#ifdef CHECK
			noOfCorrectWriteRequests += postThreadReturns.at(i).at(postThreadReturns.at(i).size() - 1);
			for (int j = 0; j < postThreadReturns.at(i).size()-1; j++) {
				noOfPostRequests += postThreadReturns.at(i).at(j);
			}
#else
		for (int j = 0; j < postThreadReturns.at(i).size(); j++) {
			noOfPostRequests += postThreadReturns.at(i).at(j);
		}
#endif
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
#ifdef CHECK
			bool requestCheck = checkPostRequest(request, reply);
			noOfCorrectRequests++;
#endif

		if (chrono::steady_clock::now() - startTime >= chrono::seconds(1) || timeUp) {
			startTime = chrono::steady_clock::now();
			noOfRequests.push_back(currentNoOfRequests);
			currentNoOfRequests = 0;
		}
	}
	noOfRequests.push_back(noOfCorrectRequests);


	simpleBarrier->count_down_and_wait();

	client.send("exit");

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
#ifdef CHECK
			bool requestCheck = checkReadRequest(request, reply);
			noOfCorrectRequests++;
#endif

		if (chrono::steady_clock::now() - startTime >= chrono::seconds(1) || timeUp) {
			startTime = chrono::steady_clock::now();
			noOfRequests.push_back(currentNoOfRequests);
			currentNoOfRequests = 0;
		}
	}
	noOfRequests.push_back(noOfCorrectRequests);

	simpleBarrier->count_down_and_wait();

	client.send("exit");

	client.CloseConnection();

	return noOfRequests;
}

inline void TestHarness::storePostRequest(string postRequest, int id) {
	if (!postRequest.empty()) {
		string postTopic = postRequest.substr(postRequest.find("@") + 1, postRequest.find("#") - postRequest.find("@") - 1);
		string postMessage = postRequest.substr(postRequest.find("#") + 1);
		dataStructure->PostFunction(postTopic, postMessage, id);
	}
}

inline bool TestHarness::checkPostRequest(string postRequest, string reply) {
	if (!postRequest.empty()) {
		string postTopic = postRequest.substr(postRequest.find("@") + 1, postRequest.find("#") - postRequest.find("@") - 1);
		string postMessage = postRequest.substr(postRequest.find("#") + 1);
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
		string postTopic = readRequest.substr(readRequest.find("@")+1, readRequest.find("#") - readRequest.find("@")-1);
		string postID = readRequest.substr(readRequest.find("#")+1);
		string message = dataStructure->ReadFunction(postTopic, stoi(postID));
		if (reply == message) {
			return true;
		}
		else {
			return false;
		}
	}
}