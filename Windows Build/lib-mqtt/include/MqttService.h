#pragma once

#include <string>
#include <iostream>

#ifdef MQTT_DLL_EXPORT
#       define MQTT_PLUGIN_EXP __declspec(dllexport) 
#else 
#       define MQTT_PLUGIN_EXP __declspec(dllimport) 
#endif 

class MQTT_PLUGIN_EXP MqttService
{
public:
	~MqttService();

	void close();
	void init();

	static MqttService* instance();
	static void destroy();

	void setHostname(const std::string &hostname);
	void subscribeToTopic(const std::string &topicName, int qos = 0);
	void unsubscribeToTopic(const std::string &topicName);

	static void debugLog(const std::string &message);

private:
	MqttService();

	static MqttService* _instance;

	void disconnectClient();
	std::string getStringResponseValue(int response) const;

	int _keepAliveInterval;
	std::string _hostname;
	std::string _clientID;
	int _tcpPort;
	bool _clientConnected;

	void* _client;//MQTTAsync
};