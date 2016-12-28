#include "stdafx.h"

#include <string>
#include <iostream>

#include "MqttService.h"

int main(int argc, char* argv[])
{
	std::cout << "Step 1 instance" << std::endl;
	MqttService::instance();
	std::cout << "Step 2 hostname" << std::endl;
	MqttService::instance()->setHostname("10.10.80.229");
	std::cout << "Step 3 init" << std::endl;
	MqttService::instance()->init();
	Sleep(3000);
	std::cout << "Step 4 subscribe to samir" << std::endl;
	MqttService::instance()->subscribeToTopic("samir");
	Sleep(1000);
	std::cout << "Step 5 subscribe to samir2" << std::endl;
	MqttService::instance()->subscribeToTopic("samir2");
	Sleep(120000);
	return 0;
}

