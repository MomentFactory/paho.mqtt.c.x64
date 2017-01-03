#include "MQTTCallbackWrapper.h"
#include "MqttService.h"

void MQTTCallbackWrapper::debugLog(const std::string &message)
{
	MqttService::instance()->debugLog(message);
}

void MQTTCallbackWrapper::onConnectionSuccess(void* context, MQTTAsync_successData* response)
{
	std::string serverHost = response->alt.connect.serverURI;
	debugLog("Mqtt client connection success to mqtt broker at " + serverHost);
}

void MQTTCallbackWrapper::onConnectionFailure(void* context, MQTTAsync_failureData* response)
{
	std::string errorMessage = response->message;
	debugLog("Mqtt client connection failure : " + errorMessage);
}

void MQTTCallbackWrapper::onConnectionLost(void* context, char* cause)
{
	debugLog("connection lost : " + std::string(cause));
}

void MQTTCallbackWrapper::onMessageDelivered(void* context, MQTTAsync_token token)
{
	debugLog("message delivered client token : " + std::to_string(token));
}

int MQTTCallbackWrapper::onMessageReceived(void *context, char *topicName, int topicLen, MQTTAsync_message* message)
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

void MQTTCallbackWrapper::onSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
	std::string* topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client subscription to '" + *topicName + "' has succeed.");
	delete topicName;
}

void MQTTCallbackWrapper::onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	std::string* topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client subscription to '" + *topicName + "' has failed.");
	delete topicName;
}

void MQTTCallbackWrapper::onUnsubscribeSuccess(void* context, MQTTAsync_successData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client unsubscription to '" + *topicName + "' has succeed.");
	delete topicName;
}

void MQTTCallbackWrapper::onUnsubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	std::string *topicName = static_cast<std::string*>(context);
	debugLog("Mqtt client unsubscription to '" + *topicName + "' has failed.");
	delete topicName;
}