#include "gps.h"

// Definisi variabel global untuk data GPS
unsigned long gITOW;
uint16_t gYear;
uint8_t gMonth, gDay, gHour, gMinute, gSecond;
uint8_t gFixType, gNumSV;
int32_t gLon, gLat, gHeight, gSpeed;

SoftwareSerial gpsSerial(D7, D6);  // D7 = RX, D6 = TX

void setupGPS() {
    gpsSerial.begin(115200);  // Baudrate untuk GPS
}

void readGPSData() {
    if (gpsSerial.available() > 0) {
        if (gpsSerial.read() == 0xB5) {         // UBX sync byte 1
            if (gpsSerial.read() == 0x62) {       // UBX sync byte 2
                if (gpsSerial.read() == 0x01) {     // Message Class (NAV)
                    if (gpsSerial.read() == 0x07) {   // Message ID (PVT)
                        if (gpsSerial.read() == 92) {   // Payload length byte 1
                            if (gpsSerial.read() == 0) {  // Payload length byte 2

                                uint8_t payload[92];
                                for (int i = 0; i < 92; i++) {
                                    payload[i] = gpsSerial.read();
                                }

                                // Parsing GPS data dan menyimpan di variabel global
                                gITOW = ((unsigned long)payload[3] << 24) | ((unsigned long)payload[2] << 16) | ((unsigned long)payload[1] << 8) | (unsigned long)payload[0];
                                gYear = ((uint16_t)payload[5] << 8) | (uint16_t)payload[4];
                                gMonth = payload[6];
                                gDay = payload[7];
                                gHour = payload[8];
                                gMinute = payload[9];
                                gSecond = payload[10];
                                gFixType = payload[20];
                                gNumSV = payload[23];
                                gLon = ((int32_t)payload[27] << 24) | ((int32_t)payload[26] << 16) | ((int32_t)payload[25] << 8) | (int32_t)payload[24];
                                gLat = ((int32_t)payload[31] << 24) | ((int32_t)payload[30] << 16) | ((int32_t)payload[29] << 8) | (int32_t)payload[28];
                                gHeight = ((int32_t)payload[35] << 24) | ((int32_t)payload[34] << 16) | ((int32_t)payload[33] << 8) | (int32_t)payload[32];
                                gSpeed = ((int32_t)payload[63] << 24) | ((int32_t)payload[62] << 16) | ((int32_t)payload[61] << 8) | (int32_t)payload[60];
                            }
                        }
                    }
                }
            }
        }
    }
}

// Fungsi getter untuk mengembalikan nilai yang telah dikonversi
double getLongitude() {
    return gLon / 10000000.0;  // Konversi ke derajat
}

double getLatitude() {
    return gLat / 10000000.0;  // Konversi ke derajat
}

double getHeight() {
    return gHeight / 1000.0;  // Konversi ke meter
}

double getGroundSpeed() {
    return gSpeed / 1000.0;  // Konversi ke m/s
}

void debugGPSData(int parameter){
  if(parameter){
    Serial.print("Time of Week: ");
    Serial.println(gITOW);
    Serial.print("Date: ");
    Serial.print(gYear);
    Serial.print("-");
    Serial.print(gMonth);
    Serial.print("-");
    Serial.println(gDay);
    Serial.print("Time: ");
    Serial.print(gHour);
    Serial.print(":");
    Serial.print(gMinute);
    Serial.print(":");
    Serial.println(gSecond);
    Serial.print("Fix Type: ");
    Serial.println(gFixType);
    Serial.print("Satellites: ");
    Serial.println(gNumSV);
    Serial.print("Longitude: ");
    Serial.println(getLongitude(), 7);
    Serial.print("Latitude: ");
    Serial.println(getLatitude(), 7);
    Serial.print("Height in meters: ");
    Serial.println(getHeight());  // in meters
    Serial.print("Ground Speed in m/s: ");
    Serial.println(getGroundSpeed());  // in m/s
  }
}
