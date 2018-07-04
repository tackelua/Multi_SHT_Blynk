// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Multi_SHT_Blynk.ino
    Created:	04-Jul-18 08:57:22
    Author:     GITH\tacke
	Version:	0.1
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <SHT1x.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#define dataPin  10
#define clockPin 11
SHT1x sht1x(dataPin, clockPin);

#define Sprintln	Serial.println
#define Sprint		Serial.print

#define VPIN_TEMPERATURE	V1
#define VPIN_HUMIDITY		V2

bool flag_read_sensor = false;

char auth[] = "72fe006d966d44df848e05c32998c0ca";

void LED_ON() {
	digitalWrite(LED_BUILTIN, LOW);
}
void LED_OFF() {
	digitalWrite(LED_BUILTIN, HIGH);
}

void wifi_manager() {
	Sprintln(F("WiFi Manager begin\n"));
	LED_ON();
	do {
		WiFiManager wifiManager;
		wifiManager.setTimeout(120);
		String AP_NAME = "MUSHROOM_SENSOR";
		Sprintln("Create AP: " + AP_NAME);
		if (!wifiManager.startConfigPortal(AP_NAME.c_str())) {
			Sprintln("failed to connect and hit timeout.\r\nTry auto connect again");
			if (WiFi.waitForConnectResult() == WL_CONNECTED)
			{
				LED_OFF();
				Sprintln(F("connected\n"));
				Sprintln(WiFi.localIP());
			}
		}
	} while (WiFi.localIP() == INADDR_NONE);
}

void WiFi_init() {
	LED_ON();//on
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);

	WiFi.printDiag(Serial);

	Sprintln(F("\nWiFi_init"));

	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		//smart_config();
		wifi_manager();
	}

	LED_OFF();
	Sprint(F("IP: "));
	Sprintln(WiFi.localIP());
}

void Blynk_init() {
	Sprintln(F("\r\nBlynk Init"));
	Blynk.config(auth, "gith.cf", 8442);
	while (Blynk.connect() != true) {
		Sprint(F("."));
		delay(100);
	}
}

BLYNK_READ(VPIN_TEMPERATURE) {
	flag_read_sensor = true;
}

void update_sensor_data() {
	LED_ON();

	Serial.println("Reading Sensor");
	float temp_c;
	float humidity;

	// Read values from the sensor
	temp_c = sht1x.readTemperatureC();
	humidity = sht1x.readHumidity();

	// Print the values to the serial port
	Serial.print("Temperature: ");
	Serial.print(temp_c, DEC);
	Serial.println("C");
	Serial.print("Humidity: ");
	Serial.print(humidity);
	Serial.println("%\r\n");

	Blynk.virtualWrite(VPIN_TEMPERATURE, temp_c);
	Blynk.virtualWrite(VPIN_HUMIDITY, humidity);

	LED_OFF();
}

void setup()
{
	Serial.begin(74880);
	WiFi_init();
	Blynk_init();

}

// Add the main program code into the continuous loop() function
void loop()
{
	Blynk.run();
	if (flag_read_sensor) {
		flag_read_sensor = false;
		update_sensor_data();
	}
	delay(1);
}
