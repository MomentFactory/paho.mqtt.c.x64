class XFRAME_PLUGIN_EXP MqttService
{
public:
	~MqttService();

	void close();
	void init();

	static MqttService* instance();
	static void destroy();
private:
	MqttService();

	static MqttService* _instance;
};