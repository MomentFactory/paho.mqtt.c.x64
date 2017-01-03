#include "MqttService.h"
#include "MQTTCallbackWrapper.h"

extern "C"
{
#include <Thread.h>
#include <MQTTAsync.h>
}

MqttService* MqttService::_instance = nullptr;

MqttService::~MqttService()
{
	debugLog("MqttService destructor");
}

MqttService::MqttService()
: _keepAliveInterval(20)
, _hostname("127.0.0.1")
, _tcpPort(1883)
, _clientID("clientMqttXagora")
, _client(nullptr)
, _clientConnected(false)
{
	debugLog("MqttService constructor");
}

MqttService* MqttService::instance()
{
	if (_instance == nullptr)
	{
		debugLog("MqttService instance creation");
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

	debugLog("MqttService instance destruction");

	_instance->close();

	delete _instance;
}

void MqttService::close()
{
	disconnectClient();

	debugLog("Mqtt client destruction started");
	MQTTAsync_destroy(&_client);
}

void MqttService::disconnectClient()
{
	debugLog("Mqtt client disconnection started");

	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int response = MQTTAsync_disconnect(_client, &opts);

	if (response == MQTTASYNC_SUCCESS)
	{
		_clientConnected = false;
	}
	debugLog("response : " + getStringResponseValue(response));
}

void MqttService::setHostname(const std::string &hostname)
{
	_hostname = hostname;
}

void MqttService::subscribeToTopic(const std::string &topicName, int qos /*= 0*/)
{
	if (_clientConnected)
	{
		debugLog("Mqtt client trying to subscribe to '" + topicName + "' topic with qos value to " + std::to_string(qos) + "...");
		MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

		opts.onSuccess = &MQTTCallbackWrapper::onSubscribeSuccess;
		opts.onFailure = &MQTTCallbackWrapper::onSubscribeFailure;
		opts.context = new std::string(topicName);

		int response = MQTTAsync_subscribe(_client, topicName.c_str(), qos, &opts);
		debugLog("response : " + getStringResponseValue(response));
	}
	else
	{
		debugLog("Cannot subscribe to '" + topicName + "' topic because mqtt client is not connected.");
	}
}

void MqttService::unsubscribeToTopic(const std::string &topicName)
{
	if (_clientConnected)
	{
		debugLog("Mqtt client trying to unsubscribe to '" + topicName + "' topic...");
		MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

		opts.onSuccess = &MQTTCallbackWrapper::onUnsubscribeSuccess;
		opts.onFailure = &MQTTCallbackWrapper::onUnsubscribeFailure;
		opts.context = new std::string(topicName);
		int response = MQTTAsync_unsubscribe(_client, topicName.c_str(), &opts);
	}
	else
	{
		debugLog("Cannot unsubscribe to '" + topicName + "' topic because mqtt client is not connected.");
	}
}

void MqttService::init()
{
	debugLog("MqttService init");

	MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_willOptions wopts = MQTTAsync_willOptions_initializer;
	MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

	debugLog("trying creation Mqtt client handle...");
	int response = MQTTAsync_create(&_client, _hostname.c_str(), _clientID.c_str(), MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
	debugLog("response : " + getStringResponseValue(response));

	if (response != MQTTASYNC_SUCCESS)
	{
		disconnectClient();
		return;
	}

	opts.keepAliveInterval = _keepAliveInterval;
	opts.cleansession = 1;

	opts.will = nullptr;
	opts.ssl = nullptr;
	
	opts.onSuccess = &MQTTCallbackWrapper::onConnectionSuccess;
	opts.onFailure = &MQTTCallbackWrapper::onConnectionFailure;

	opts.context = _client;

	response = MQTTAsync_setCallbacks(_client, NULL, &MQTTCallbackWrapper::onConnectionLost, &MQTTCallbackWrapper::onMessageReceived, &MQTTCallbackWrapper::onMessageDelivered);

	debugLog("trying Mqtt client connection...");
	response = MQTTAsync_connect(_client, &opts);
	debugLog("response : " + getStringResponseValue(response));

	if (response != MQTTASYNC_SUCCESS)
	{
		disconnectClient();
	}
	else
	{
		_clientConnected = true;
	}
}

void MqttService::debugLog(const std::string &message)
{
	std::cout << "---" << message << "---" << std::endl;
}

std::string MqttService::getStringResponseValue(int response) const
{
	std::string stringResponse;

	switch (response)
	{
	case MQTTASYNC_SUCCESS:
		stringResponse = "MQTTASYNC_SUCCESS";
		break;
	case MQTTASYNC_FAILURE:
		stringResponse = "MQTTASYNC_FAILURE";
		break;
	case MQTTASYNC_PERSISTENCE_ERROR:
		stringResponse = "MQTTASYNC_PERSISTENCE_ERROR";
		break;
	case MQTTASYNC_DISCONNECTED:
		stringResponse = "MQTTASYNC_DISCONNECTED";
		break;
	case MQTTASYNC_MAX_MESSAGES_INFLIGHT:
		stringResponse = "MQTTASYNC_MAX_MESSAGES_INFLIGHT";
		break;
	case MQTTASYNC_BAD_UTF8_STRING:
		stringResponse = "MQTTASYNC_BAD_UTF8_STRING";
		break;
	case MQTTASYNC_NULL_PARAMETER:
		stringResponse = "MQTTASYNC_NULL_PARAMETER";
		break;
	case MQTTASYNC_TOPICNAME_TRUNCATED:
		stringResponse = "MQTTASYNC_TOPICNAME_TRUNCATED";
		break;
	case MQTTASYNC_BAD_STRUCTURE:
		stringResponse = "MQTTASYNC_BAD_STRUCTURE";
		break;
	case MQTTASYNC_BAD_QOS:
		stringResponse = "MQTTASYNC_BAD_QOS";
		break;
	case MQTTASYNC_NO_MORE_MSGIDS:
		stringResponse = "MQTTASYNC_NO_MORE_MSGIDS";
		break;
	case MQTTASYNC_OPERATION_INCOMPLETE:
		stringResponse = "MQTTASYNC_OPERATION_INCOMPLETE";
		break;
	case MQTTASYNC_MAX_BUFFERED_MESSAGES:
		stringResponse = "MQTTASYNC_MAX_BUFFERED_MESSAGES";
		break;
	default:
		stringResponse = "undefined reponse";
		break;
	}

	return stringResponse;
}

