#include <Arduino.h>
#include <WiFiS3.h>
#include <arduino_secrets.h>
#include <SparkFun_SCD4x_Arduino_Library.h>
#include <Wire.h>


#define LED_GROEN 4
#define LED_GEEL 2
#define LED_ROOD 3

SCD4x mySensor;
int status = WL_IDLE_STATUS;

// Millis
unsigned long millisDataSturen = 0;
unsigned long millisTime = 0;


// Function to initialize Led's
void initLed()
{
  pinMode(LED_GROEN, OUTPUT);
  pinMode(LED_GEEL, OUTPUT);
  pinMode(LED_ROOD, OUTPUT);

  digitalWrite(LED_GROEN, LOW); // Initialize LED_GROEN as off
  digitalWrite(LED_GEEL, LOW);  // Initialize LED_GEEL as off
  digitalWrite(LED_ROOD, LOW);  // Initialize LED_ROOD as off
}

void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// Function that collects data from the sensor and sends it to sheets
void sendDataSheets()
{
  digitalWrite(LED_ROOD, HIGH);

  WiFiSSLClient client;

  // Get the values from the sensor
  String co2Value = String(mySensor.getCO2());
  String temperatureValue = String(mySensor.getTemperature(), 1);
  String humidityValue = String(mySensor.getHumidity(), 1);

  // Construct the final URL for the Google Sheets script
  String urlFinal = GOOGLE_APPS_SCRIPT_URL + GOOGLE_SCRIPT_ID + "/exec?co2=" + co2Value + "&temperature=" + temperatureValue + "&humidity=" + humidityValue;
  if (client.connect("script.google.com", 443))
  {
    Serial.println("connected to server");

    // Send HTTP GET request
    client.println("GET " + urlFinal + " HTTP/1.1");
    client.println("Host: script.google.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  else
  {
    client.stop();
    Serial.println("No client connect");
  }
  digitalWrite(LED_ROOD, LOW);
}

void setup()
{
  initLed();
  Serial.begin(9600);

  digitalWrite(LED_GROEN, HIGH);

  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
    digitalWrite(LED_GEEL, HIGH);
  }

  printWifiStatus();

  Wire1.begin(); // Join I2C bus
  mySensor.begin();

  Serial.println("\nStarting connection to server...");
}

void loop()
{
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
    digitalWrite(LED_GEEL, HIGH);
  }

  Serial.println("still running 1");
  
  if (millis() >= millisDataSturen + (60000 * 15))
  {
    Serial.println("still running 2");
    millisDataSturen = millis();
    sendDataSheets();
  }
  Serial.println("still running 3");
}
