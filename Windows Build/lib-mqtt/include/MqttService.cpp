#include "MqttServicePch.h"
#include "MqttService.h"

MqttService* MqttService::_instance = nullptr;

MqttService::~MqttService()
{
	std::cout << "MqttService Destructor" << std::endl;
}

MqttService::MqttService()
{
	std::cout << "MqttService Constructor" << std::endl;
}

MqttService* MqttService::instance()
{
	if (_instance == nullptr)
	{
		std::cout << "MqttService instance creation" << std::endl;
		_instance = new MqttService;
	}

	return _instance;
}

void MqttService::destroy()
{
	if (_instance == nullptr)
	{
		return;
	}

	std::cout << "MqttService destroy" << std::endl;

	_instance->close();

	delete _instance;
}

void MqttService::close()
{
	std::cout << "MqttService close" << std::endl;
}

void MqttService::init()
{
	std::cout << "MqttService init" << std::endl;

	std::string url = "";
	std::string clientID = "test";

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc = 0;

	rc = MQTTClient_create(&client, url.c_str(), clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);

	std::cout << "yooooo" << std::endl;
}

