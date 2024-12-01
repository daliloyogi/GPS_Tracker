#ifndef GPS_H
#define GPS_H

#include <SoftwareSerial.h>

// Deklarasi variabel global untuk data GPS
extern unsigned long gITOW;
extern uint16_t gYear;
extern uint8_t gMonth, gDay, gHour, gMinute, gSecond;
extern uint8_t gFixType, gNumSV;
extern int32_t gLon, gLat, gHeight, gSpeed;

// Deklarasi fungsi untuk setup dan pembacaan data GPS
void setupGPS();
void readGPSData();
void debugGPSData(int parameter);

// Getter untuk nilai yang sudah dikonversi
double getLongitude();
double getLatitude();
double getHeight();
double getGroundSpeed();

#endif
