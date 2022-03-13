#pragma once
#include "NTPClient.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <Arduino.h>
#include "WiFi.h"
#include <WiFiMulti.h>
#include "PubSubClient.h"
#include "AccessPoints.h"
#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <MFRC522Extended.h>
#include <SPI.h> //library responsible for communicating of SPI bus
#include <string>  
#include <ESP32Servo.h>
#include "userdatabase.h"

WiFiMulti wifiMulti;
WiFiClient espClient;
static PubSubClient client(espClient);
