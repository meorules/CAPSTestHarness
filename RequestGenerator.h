#pragma once
#include <string>
#include <random>
#include "UnorderedMap.h"

using namespace std;

class RequestGenerator
{
public:

  RequestGenerator();
  ~RequestGenerator();
  string generateRandomReadRequest();
  string generateReadRequest(DataStructureAPI* structure);
  string generatePostRequest();

private:
  string generateRandomString(int size);
  string read = "READ";
  string post = "POST";
  string randomString;
  int topicCounter;
  int messageCounter;


};


inline RequestGenerator::RequestGenerator() :topicCounter(0),messageCounter(0) {
  int randomNUmber = rand() % 140 - 20;
  randomString = generateRandomString(randomNUmber);
}

inline RequestGenerator::~RequestGenerator() {

}


inline string RequestGenerator::generateRandomReadRequest(){
  string readRequest;
  readRequest.append(read + "@");
  if (messageCounter == 5)
    topicCounter++;
  messageCounter = (messageCounter + 1) % 5;
  readRequest.append(topicCounter + "#" + messageCounter);
  return readRequest;

}

inline string RequestGenerator::generateReadRequest(DataStructureAPI* structure) {
  
  //Look through the structure and find a possible read
  string topic = structure->findReadTopic();
  if (topic != "") {
    string readRequest;
    readRequest.append(read + "@");
    string messageID = to_string(structure->findReadMessage(topic));
    readRequest.append(topic + "#" + messageID);
    return readRequest;
  }
  else {
    return generateRandomReadRequest();
  }

}

inline string RequestGenerator::generatePostRequest() {
  string postRequest;
  postRequest.append(post + "@");
  if (messageCounter == 5)
    topicCounter++;
  messageCounter = (messageCounter + 1) % 5;
  postRequest.append(topicCounter + "#" + messageCounter);
  return postRequest;
}

//Code to generate a random string of characters was gotten from https://inversepalindrome.com/blog/how-to-create-a-random-string-in-cpp
inline string RequestGenerator::generateRandomString(int size)
{
  const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

  std::string random_string;

  for (std::size_t i = 0; i < size; ++i)
  {
    random_string += CHARACTERS[distribution(generator)];
  }

  return random_string;
}
