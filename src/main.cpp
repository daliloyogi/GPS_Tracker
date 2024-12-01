#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <SoftwareSerial.h>
#include "gps.h"
#include "WiFiConfig.h"

// Konfigurasi Bot Telegram
#define BOT_TOKEN "7368137697:AAEqNKDsEDJ3NDgKTTYQwZE30oMty8HbdPU"
#define CHAT_ID "-1002152511231"

// Pin Output
const int relayPin = D1;
const int buzzerPin = D2;
const int ledPin = D4;

// Variabel waktu
unsigned long lastWifiCheck = 0;
unsigned long lastGPSRead = 0;
unsigned long buzzerStartTime = 0;
unsigned long previousmillis = 0;
bool buzzerState = false;
bool isBuzzerActive = false;
bool sentLocationUpdate = false;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Variabel GPS
double latitude, longitude, satellite;
uint8_t hour;

void handleNewMessages(const telegramMessage &msg)
{
  String text = msg.text;

  if (text == "/Start Engine")
  {
    digitalWrite(relayPin, LOW);
    bot.sendMessage(CHAT_ID, "The engine has been turned on!", "");
  }
  else if (text == "/Stop Engine")
  {
    isBuzzerActive = true;
    buzzerStartTime = millis();
    bot.sendMessage(CHAT_ID, "The engine will be turned off in 30 seconds", "");
  }
  else if (text == "/Force Stop Engine")
  {
    digitalWrite(relayPin, HIGH);
    bot.sendMessage(CHAT_ID, "The engine has been forced to stop!", "");
  }
  else if (text == "/Track Location")
  {
    if (gFixType > 0)
    {
      // Generate GPS date
      String gpsDate = "Date: " + String(gDay) + "-" + String(gMonth) + "-" + String(gYear) + "\n";

      // Generate GPS time
      String gpsTime = "Time (GMT+7): " + String(hour) + ":" + String(gMinute) + ":" + String(gSecond) + "\n";

      // Generate GPS location
      String googleMapsLink = "Location: \nhttps://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);

      // Send Telegram
      bot.sendMessage(CHAT_ID, gpsDate + gpsTime + googleMapsLink, "");
    }
    else
    {
      String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);

      bot.sendMessage(CHAT_ID, "Sorry, location point is not yet available\n" + googleMapsLink, "");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setupGPS();

  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(relayPin, LOW);  // Relay off pada awalnya
  digitalWrite(buzzerPin, LOW); // Buzzer off pada kondisi awal
  digitalWrite(ledPin, LOW);    // LED off pada kondisi awal

  connectWifi();

  client.setInsecure();
  bot.sendMessage(CHAT_ID, "The device has started!", "");
}

void loop()
{
  // Cek rutin WiFi setiap 5 detik
  if (millis() - lastWifiCheck >= 5000)
  {
    checkWifiConnection();
    lastWifiCheck = millis();
  }

  // Baca data GPS setiap 1000MS
  if (millis() - lastGPSRead >= 1000)
  {
    readGPSData();
    // Simpan data GPS ke variabel
    latitude = getLatitude();
    longitude = getLongitude();
    satellite = gNumSV;
    // Simpan Waktu GPS
    hour = (gHour + 7) % 24;

    // Indikator GPSFix
    if (gFixType > 0)
    {
      digitalWrite(ledPin, HIGH);
      if (!sentLocationUpdate)
      {
        bot.sendMessage(CHAT_ID, "Location point available", "");
        sentLocationUpdate = true;
      }
    }
    else
    {
      digitalWrite(ledPin, LOW);
      sentLocationUpdate = false;
    }

    // debugGPSData(1);
  }

  // Buzzer dinyalakan dengan interval 0.5s selama 30s
  if (isBuzzerActive)
  {
    for (int i = 0; i < 60; i++)
    {
      digitalWrite(buzzerPin, HIGH);
      delay(500);                   
      digitalWrite(buzzerPin, LOW); 
      delay(500);                   
    }

    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, HIGH);                          
    isBuzzerActive = false;                                
    bot.sendMessage(CHAT_ID, "The engine has been turned off!", ""); 
  }

  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  for (int i = 0; i < numNewMessages; i++)
  {
    handleNewMessages(bot.messages[i]);
  }
}
