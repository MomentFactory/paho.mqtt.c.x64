#include "stdafx.h"

#include <string>
#include <iostream>

extern "C"
{
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"
}

int main(int argc, char* argv[])
{
	std::cout << "start";

	std::string url = "";
	std::string clientID = "test";

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc = 0;

	rc = MQTTClient_create(&client, url.c_str(), clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);

	return 0;
}

