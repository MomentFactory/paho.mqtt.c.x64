#include "stdafx.h"

#include <string>
#include <iostream>

#include "MqttService.h"

int main(int argc, char* argv[])
{
	std::cout << "Step 1" << std::endl;
	MqttService::instance();
	Sleep(1000);
	std::cout << "Step 2" << std::endl;
	MqttService::instance()->init();
	Sleep(10000);
	return 0;
}

