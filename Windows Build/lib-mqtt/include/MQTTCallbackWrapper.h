#pragma once

class MQTTCallbackWrapper
{
public:
	static void onConnectionSuccess(void* context, MQTTAsync_successData* response); //static because that the way to receive callback from c
	static void onConnectionFailure(void* context, MQTTAsync_failureData* response);
	static void onConnectionLost(void* context, char* cause);
	static void onMessageDelivered(void* context, MQTTAsync_token token);
	static int onMessageReceived(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
	static void onSubscribeSuccess(void* context, MQTTAsync_successData* response);
	static void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
	static void onUnsubscribeSuccess(void* context, MQTTAsync_successData* response);
	static void onUnsubscribeFailure(void* context, MQTTAsync_failureData* response);
	static void debugLog(const std::string &message);
};