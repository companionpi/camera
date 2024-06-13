#include "WifiCam.hpp"
#include <WiFi.h>

static const char *SSID = "<YOUR WIFI SSID>";
static const char *PASSWORD = "<YOUR WIFI PASSWORD>";

IPAddress localip(192, 168, 0, 184);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

esp32cam::Resolution initialResolution;
WebServer server(80);

void reboot()
{
	delay(5000);
	ESP.restart();
}

void setup()
{
	Serial.begin(115200);
	Serial.println();
	delay(2000);

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	if (!WiFi.config(localip, gateway, subnet, primaryDNS, secondaryDNS))
		Serial.println("⛔ Unable to configure STA");
	WiFi.begin(SSID, PASSWORD);

	if (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.printf("⛔ Wi-Fi failure %d\n", WiFi.status());
		reboot();
	}
	Serial.println("✅ Wi-Fi connected");

	{
		using namespace esp32cam;
		initialResolution = Resolution::find(1024, 768);

		Config cfg;
		cfg.setPins(pins::AiThinker);
		cfg.setResolution(initialResolution);
		cfg.setJpeg(80);

		bool ok = Camera.begin(cfg);
		if (!ok)
		{
			Serial.println("⛔ Unable to initialize the camera module");
			reboot();
		}
		Serial.println("✅ Camera module active");
	}

	Serial.println("✅ Camera started");
	Serial.print("🔗 http://");
	Serial.println(WiFi.localIP());

	addRequestHandlers();
	server.begin();
}

void loop()
{
	server.handleClient();
}