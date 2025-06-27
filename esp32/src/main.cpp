#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

// Replace these with your actual Wi-Fi credentials
const char *ssid = "Amped_B1912_B7DC";
const char *password = "7601cbab";

WiFiServer server(80);

// Blink LED twice as a Wi-Fi connected debug signal
void blinkTwice()
{
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // wait for Serial to become ready (USB CDC delay workaround)
  }
  Serial.println("Serial ready...");
  Serial.println("\nConnecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected.");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  blinkTwice(); // Visual confirmation of connection

  server.begin();
}

void loop()
{
  // static unsigned long lastPrintTime = 0;
  // unsigned long now = millis();

  // // Print current IP every 5 seconds
  // if (now - lastPrintTime > 5000)
  // {
  //   lastPrintTime = now;

  //   if (WiFi.status() == WL_CONNECTED)
  //   {
  //     Serial.print("Current IP: ");
  //     Serial.println(WiFi.localIP());
  //   }
  //   else
  //   {
  //     Serial.println("Wi-Fi not connected");
  //   }
  // }

  // Handle HTTP client requests
  WiFiClient client = server.available();
  if (!client)
    return;

  Serial.println("Client connected");
  String request = "";
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      request += c;
      if (request.endsWith("\r\n\r\n"))
        break;
    }
  }

  Serial.println("Request: " + request);

  String response = "LED command received";
  if (request.indexOf("GET /led/on") >= 0)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    response = "LED turned ON";
  }
  else if (request.indexOf("GET /led/off") >= 0)
  {
    digitalWrite(LED_BUILTIN, LOW);
    response = "LED turned OFF";
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/plain");
  client.println("Access-Control-Allow-Origin: *");
  client.println();
  client.println(response);
  client.println();

  delay(1);
  client.stop();
  Serial.println("Client disconnected\n");
}
