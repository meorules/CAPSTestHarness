#pragma once
#include <iostream>
using namespace std;

class mathAverages
{
public:
	mathAverages();
	mathAverages(int noOfPostThreads, int noOfReadThreads, int noOfPostRequests, int noOfReadRequests, float time, int noOfCorrectWriteRequests = -1, int noOfCorrectReadRequests = -1);
	~mathAverages();

	friend ostream& operator<<(ostream& os, const mathAverages& mA);


private:

	int noOfPostThreads;
	int noOfReadThreads;
	int noOfPostRequests;
	int noOfReadRequests;
	int noOfCorrectWriteRequests;
	int noOfCorrectReadRequests;
	float time;
	float percentageOfCorrectWriteRequests;
	float percentageOfCorrectReadRequests;
	float percentageOfCorrectRequests;
	float noOfPostRequestsPerSecond;
	float noOfReadRequestsPerSecond;
	float noOfPostRequestsPerThreadPerSecond;
	float noOfReadRequestsPerThreadPerSecond;
	float noOfRequestsPerThreadPerSecond;

};

inline mathAverages::mathAverages(int noOfPostThreads, int noOfReadThreads, int noOfPostRequests, int noOfReadRequests, float time, int noOfCorrectWriteRequests, int noOfCorrectReadRequests) : noOfPostThreads(noOfPostThreads), noOfReadThreads(noOfReadThreads), noOfPostRequests(noOfPostRequests), noOfReadRequests(noOfReadRequests), noOfCorrectWriteRequests(noOfCorrectWriteRequests), noOfCorrectReadRequests(noOfCorrectReadRequests), time(time) {
	float percentageOfCorrectWriteRequests = noOfCorrectWriteRequests / noOfPostRequests;
	float percentageOfCorrectReadRequests = noOfCorrectReadRequests / noOfReadRequests;
	float percentageOfCorrectRequests = (noOfCorrectReadRequests + noOfCorrectWriteRequests) / (noOfPostRequests + noOfReadRequests);
	float noOfPostRequestsPerSecond = noOfPostRequests / time;
	float noOfReadRequestsPerSecond = noOfReadRequests / time;
	float noOfPostRequestsPerThreadPerSecond = noOfPostRequestsPerSecond / noOfPostThreads;
	float noOfReadRequestsPerThreadPerSecond = noOfReadRequestsPerSecond / noOfReadThreads;
	float noOfRequestsPerThreadPerSecond = (noOfPostRequests + noOfReadRequests) / (noOfPostThreads + noOfReadThreads) / time;
}

inline mathAverages::~mathAverages() {

}


inline ostream& operator<<(ostream& os, const mathAverages& mA)
{
	os << "Number of Post Requests: " << mA.noOfPostRequests << "\n";
	os << "Number of Read Requests: " << mA.noOfReadRequests << "\n";

	if (mA.noOfCorrectWriteRequests != -1 && mA.noOfCorrectReadRequests != -1) {
		os << "Number of Correct Post Requests: " << mA.noOfCorrectWriteRequests << "\n";
		os << "Number of Correct Read Requests: " << mA.noOfCorrectReadRequests << "\n";

		os << "Percentage Of Correct Write Requests: " << mA.percentageOfCorrectWriteRequests << "\n";
		os << "Percentage Of Correct Read Requests: " << mA.percentageOfCorrectReadRequests << "\n";
		os << "Percentage Of Correct Requests: " << mA.percentageOfCorrectRequests << "\n";
	}

	os << "Number of Post Requests Per Second: " << mA.noOfPostRequestsPerSecond << "\n";
	os << "Number of Read Requests Per Second: " << mA.noOfReadRequestsPerSecond << "\n";
	os << "Number of Post Requests Per Second Per Thread: " << mA.noOfPostRequestsPerThreadPerSecond << "\n";
	os << "Number of Read Requests Per Second Per Thread: " << mA.noOfReadRequestsPerThreadPerSecond << "\n";
	os << "Number of Requests Per Second Per Thread" << mA.noOfRequestsPerThreadPerSecond << "\n";

	return os;
}