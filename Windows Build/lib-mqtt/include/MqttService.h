#pragma once

#ifndef XFRAME_PLUGIN_EXPORTS //if the lib is used by an external project we need to declare those class to compile
	class MQTTAsync_successData;
	class MQTTAsync_failureData;
	class MQTTAsync_message;

	typedef void* MQTTAsync;
	typedef int MQTTAsync_token;
#endif 

class XFRAME_PLUGIN_EXP MqttService
{
public:
	~MqttService();

	void close();
	void init();

	static MqttService* instance();
	static void destroy();

	void setHostname(std::string hostname);
	void subscribeToTopic(std::string topicName, int qos = 0);
	void unsubscribeToTopic(std::string topicName);
private:
	MqttService();

	static MqttService* _instance;

	static void onConnectionSuccess(void* context, MQTTAsync_successData* response); //static because that the way to receive callback from c
	static void onConnectionFailure(void* context, MQTTAsync_failureData* response);
	static void onConnectionLost(void* context, char *cause);
	static void onMessageDelivered(void *context, MQTTAsync_token token);
	static int onMessageReceived(void *context, char *topicName, int topicLen, MQTTAsync_message* message);
	static void onSubscribeSuccess(void* context, MQTTAsync_successData* response);
	static void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
	static void onUnsubscribeSuccess(void* context, MQTTAsync_successData* response);
	static void onUnsubscribeFailure(void* context, MQTTAsync_failureData* response);

	static void debugLog(std::string message);

	void disconnectClient();
	std::string getStringResponseValue(int response);

	int _keepAliveInterval;
	std::string _hostname;
	std::string _clientID;
	int _tcpPort;
	bool _clientConnected;
	MQTTAsync _client;
};