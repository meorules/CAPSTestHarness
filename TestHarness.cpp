#include <iostream>

#include "TCPClient.h"

#define DEFAULT_PORT 12345


/*TODO
*Create the Random Request Generator
* Create a global timer to time 10 seconds and then send a stop signal to all threads
* Use a barrier when creating all threads to ensure the 10 seconds starts when the threads are all connected to the clients
* Use a timer in each thread to find their own averages of sending requests
* Use a math class to find all the various averages and print them out to the screen 
*/

/* @brief Main function used to run the test harness
*
* @param 
* 
*/
int main(int argc, char** argv)
{
	// Validate the parameters
	if (argc != 6) {
		printf("Parameters must be: \n Server IP \n Number of POST threads \n Number of READ threads \nTime Duration (in seconds) \n Throttled - 0(No) and 1(yes) ", argv[0]);
		return 1;
	}
	char* serverIP = argv[1];



	TCPClient client(serverIP, DEFAULT_PORT);
	std::string request;

	client.OpenConnection();

	do {
		request = "";
		std::cout << "Enter string to send to server or \"exit\" (without quotes to terminate): ";
		std::getline(std::cin, request);

		std::cout << "String sent: " << request << std::endl;
		std::cout << "Bytes sent: " << request.size() << std::endl;

		std::string reply = client.send(request);

		std::cout << "String returned: " << reply << std::endl;
		std::cout << "Bytes received: " << reply.size() << std::endl;
	} while (request != "exit" && request != "EXIT");

	client.CloseConnection();

	return 0;
}
