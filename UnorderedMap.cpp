#include "UnorderedMap.h"

UnorderedMap::UnorderedMap() {
  dataStructure = new unordered_map<string, string*>;
  keyList = new vector<string>;

}
UnorderedMap::~UnorderedMap() {
  delete dataStructure;
  delete keyList;
}


/**
 * @param Topic is the topic for which the post needs to take place, topics are created if they dont exist.
 * message is the message added the topic specified.
 * @return an int value corresponding to the id of the message which was saved
 */
void UnorderedMap::PostFunction(string topic, string message, int id)
{
  if (structNotEmpty()) {
    if (TopicExists(topic)) {
      std::unique_lock<std::shared_mutex> mutex(lock);
      string* topicArray = dataStructure->at(topic);
      if (topicArray == nullptr)
      {
        topicArray = new string[50];
        topicArray[id] = message;
        dataStructure->try_emplace(topic, topicArray);
      }
    }
    else {
      std::unique_lock<std::shared_mutex> mutex(lock);
      string* topicArray = new string[50];
      topicArray[id] = message;
      keyList->push_back(topic);
      dataStructure->try_emplace(topic, topicArray);
    }
  }
  else {
    std::unique_lock<std::shared_mutex> mutex(lock);
    string* topicArray = new string[50];
    topicArray[id] = message;
    keyList->push_back(topic);

    dataStructure->try_emplace(topic, topicArray);

  }

}

/*
 * @return a string containing the topics list seperated by @ and #
 */
string UnorderedMap::ListFunction()
{
  string topicList = "";
  if (structNotEmpty()) {
    {
      shared_lock<shared_mutex> mutex(lock);
      unordered_map<string, string*>::iterator it = dataStructure->begin();
      while (it != dataStructure->end()) {
        topicList.append(it->first + "#");
        it++;
      }
    }
    if (topicList != "") {
      topicList.pop_back();
    }
  }
  return topicList;

}


/**
 * @param Takes a topic to check
 * @return an int value corresponding to the number of messages for the topic provided,
 * returns 0 if topic does not exist
 */
int UnorderedMap::CountFunction(string topic)
{
  int messageCount = 0;
  if (structNotEmpty()) {
    if (TopicExists(topic)) {
      shared_lock<shared_mutex> mutex(lock);
      messageCount = dataStructure->at(topic)->size();
    }
  }
  return messageCount;

}

const string UnorderedMap::findReadTopic() {
  string toReturn = "";
  if (keyList->size() != 0) {
    int randomPlace = rand() % keyList->size();
    return keyList->at(randomPlace);
  }
  else {
    return toReturn;
  }
}

const int UnorderedMap::findReadMessage(const string topic) {
  int toReturn = 0;
  int size = dataStructure->at(topic)->size();
  if (size != 0) {
    return rand() % size;
  }
  else {
    return -1;
  }
}


/**
 * @param Topic corresponding to the post, the messageID for the int id where the message is saved
 * @return the string value of the message, if no message is found, a blank string will be returned
 */
string UnorderedMap::ReadFunction(string topic, int messagedID)
{
  string message = "";
  if (structNotEmpty()) {
    if (TopicExists(topic)) {
      shared_lock<shared_mutex> mutex(lock);
      if (dataStructure->at(topic)->size() > messagedID) {
        message = dataStructure->at(topic)[messagedID];
      }
    }
  }
  return message;
}

/**
 * @param Topic to be searched for
 * @return bool value, True if the topic is found, False if the topic does not exist
 */
bool UnorderedMap::TopicExists(string topic) {
  {
    shared_lock<shared_mutex> mutex(lock);
    return dataStructure->contains(topic);
  }
}

bool UnorderedMap::structNotEmpty() {
  {
    shared_lock<shared_mutex> mutex(lock);
    return !dataStructure->empty();
  }
}