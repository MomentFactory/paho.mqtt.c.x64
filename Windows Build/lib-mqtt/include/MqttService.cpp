#include "MqttServicePch.h"
#include "MqttService.h"

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
, _client(NULL)
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

void MqttService::setHostname(std::string hostname)
{
	_hostname = hostname;
}

void MqttService::onConnectionSuccess(void* context, MQTTAsync_successData* response)
{
	std::string serverHost = response->alt.connect.serverURI;
	debugLog("Mqtt client connection success to mqtt broker at " + serverHost);
}

void MqttService::onConnectionFailure(void* context, MQTTAsync_failureData* response)
{
	std::string errorMessage = response->message;
	debugLog("Mqtt client connection failure : " + errorMessage);
}

void MqttService::onConnectionLost(void* context, char *cause)
{
	debugLog("connection lost : " + std::string(cause));
}

void MqttService::onMessageDelivered(void *context, MQTTAsync_token token)
{
	debugLog("message delivered client token : " + std::to_string(token));
}

int MqttService::onMessageReceived(void *context, char *topicName, int topicLen, MQTTAsync_message* message)
{
	char* payloadptr = (char*)message->payload;
	std::string payload = "";

	for (int i = 0; i < message->payloadlen; i++)
	{
		payload += payloadptr[i];
	}

	debugLog("payload received for '" + std::string(topicName) + "' topic : '" + payload + "' (payload length : " + std::to_string(message->payloadlen) + ")");

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);

	return 1;
}

void MqttService::onSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client subscription to '" + *topicName + "' has succeed.");
}

void MqttService::onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client subscription to '" + *topicName + "' has failed.");
}

void MqttService::onUnsubscribeSuccess(void* context, MQTTAsync_successData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client unsubscription to '" + *topicName + "' has succeed.");
}

void MqttService::onUnsubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client unsubscription to '" + *topicName + "' has failed.");
}

void MqttService::subscribeToTopic(std::string topicName, int qos /*= 0*/)
{
	if (_clientConnected)
	{
		debugLog("Mqtt client trying to subscribe to '" + topicName + "' topic with qos value to " + std::to_string(qos) + "...");
		MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

		opts.onSuccess = &MqttService::onSubscribeSuccess;
		opts.onFailure = &MqttService::onSubscribeFailure;
		opts.context = &topicName;

		int response = MQTTAsync_subscribe(_client, topicName.c_str(), qos, &opts);
		debugLog("response : " + getStringResponseValue(response));
	}
	else
	{
		debugLog("Cannot subscribe to '" + topicName + "' topic because mqtt client is not connected.");
	}
}

void MqttService::unsubscribeToTopic(std::string topicName)
{
	if (_clientConnected)
	{
		debugLog("Mqtt client trying to unsubscribe to '" + topicName + "' topic...");
		MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

		opts.onSuccess = &MqttService::onUnsubscribeSuccess;
		opts.onFailure = &MqttService::onUnsubscribeFailure;
		opts.context = &topicName;
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

	opts.will = NULL;
	opts.ssl = NULL;
	
	opts.onSuccess = &MqttService::onConnectionSuccess;
	opts.onFailure = &MqttService::onConnectionFailure;

	opts.context = _client;

	response = MQTTAsync_setCallbacks(_client, NULL, &MqttService::onConnectionLost, &MqttService::onMessageReceived, &MqttService::onMessageDelivered);

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

void MqttService::debugLog(std::string message)
{
	std::cout << "---" << message << "---" << std::endl;
}

std::string MqttService::getStringResponseValue(int response)
{
	std::string stringResponse = "";

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

