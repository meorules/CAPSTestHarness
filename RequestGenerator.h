#pragma once
#include <string>

using namespace std;

class RequestGenerator
{
public:

  RequestGenerator();
  ~RequestGenerator();
  string generateReadRequest();
  string generatePostRequest();

private:
  string generateRandomString(int size);
  string read = "READ";
  string post = "POST";
  string randomString;
  int topicCounter;
  int messageCounter;

};


inline RequestGenerator::RequestGenerator() {

}

inline RequestGenerator::~RequestGenerator() {


inline string RequestGenerator::generateReadRequest() {


}

inline string RequestGenerator::generatePostRequest() {

}