#include "WiFiconfig.h"

// SSID and Password
const char *ssid = "GPS_TRACKER";
const char *password = "&GPS_TRACKER&;";

// fungsi untuk koneksi WiFi
void connectWifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// fungsi untuk cek koneksi WiFi
void checkWifiConnection()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected!");
    Serial.println("Trying to reconnect");
    connectWifi();
  }
}
