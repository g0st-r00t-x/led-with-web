#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

// Web Server on port 80
ESP8266WebServer server(80);

// Pin definitions for ESP8266
const int LED_GREEN_PIN = D5;   // GPIO4 Lampu kamar 1
const int LED_YELLOW_PIN = D2;  // GPIO0 Lampu kamar_mandi
const int LED_RED_PIN = D1;     // GPIO2 Lampu ruang_tamu

// Global variables
unsigned long previousMillis = 0;
const unsigned long NORMAL_INTERVAL = 1000;
const unsigned long FAST_INTERVAL = 200;
unsigned long alternateDelay = 500;
int ledState = 0;
bool ledEnabled = false;
bool ledKamar = false;
bool ledKamarMandi = false;
bool ledRuangTamu = false;
bool fastBlink = false;
bool alternateMode = false;

void sendCORSHeaders() {
    // Get the origin from the request header
    String origin = server.header("Origin");
    
    if (origin.length() > 0) {
        // Send back the requesting origin instead of wildcard
        server.sendHeader("Access-Control-Allow-Origin", origin);
        server.sendHeader("Access-Control-Allow-Credentials", "true");
    } else {
        // If no origin header, use wildcard but don't allow credentials
        server.sendHeader("Access-Control-Allow-Origin", "*");
    }
    
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    
    if (server.method() == HTTP_OPTIONS) {
        server.send(204); // No content for preflight requests
    }
}

void handleStatusRequest() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    DynamicJsonDocument doc(256);
    doc["ledEnabled"] = ledEnabled;
    doc["ledKamar"] = ledKamar;
    doc["ledKamarMandi"] = ledKamarMandi;
    doc["ledRuangTamu"] = ledRuangTamu;
    doc["fastBlink"] = fastBlink;
    doc["alternateMode"] = alternateMode;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleToggleRequest() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    ledEnabled = !ledEnabled;
    alternateMode = false;
    fastBlink = false;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["ledEnabled"] = ledEnabled;
    doc["fastBlink"] = fastBlink;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleLedKamar() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    ledKamar = !ledKamar;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["ledKamar"] = ledKamar;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleLedKamarMandi() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    ledKamarMandi = !ledKamarMandi;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["ledKamarMandi"] = ledKamarMandi;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleLedRuangTamu() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    ledRuangTamu = !ledRuangTamu;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["ledRuangTamu"] = ledRuangTamu;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleSpeedRequest() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    fastBlink = !fastBlink;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["fastBlink"] = fastBlink;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void handleAlternateRequest() {
    sendCORSHeaders(); // Add CORS headers to the response
    
    alternateMode = !alternateMode;
    
    DynamicJsonDocument doc(128);
    doc["success"] = true;
    doc["alternateMode"] = alternateMode;
    
    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
}

void setupRoutes() {
    // Handler untuk endpoint utama
    server.on("/api/status", HTTP_GET, handleStatusRequest);
    server.on("/api/toggle", HTTP_POST, handleToggleRequest);
    server.on("/api/kamar", HTTP_POST, handleLedKamar);
    server.on("/api/kamarmandi", HTTP_POST, handleLedKamarMandi);
    server.on("/api/ruangtamu", HTTP_POST, handleLedRuangTamu);
    server.on("/api/speed", HTTP_POST, handleSpeedRequest);
    server.on("/api/alternate", HTTP_POST, handleAlternateRequest);
    
    // Add OPTIONS handler for all routes
    server.onNotFound([]() {
        if (server.method() == HTTP_OPTIONS) {
            sendCORSHeaders();
        } else {
            server.send(404, "text/plain", "Not found");
        }
    });
}


void updateLEDs() {
    unsigned long currentMillis = millis();
    unsigned long interval = fastBlink ? FAST_INTERVAL : NORMAL_INTERVAL;

    if (alternateMode && ledEnabled) {
        // Alternate mode: LEDs blink in sequence
        if (currentMillis - previousMillis >= (fastBlink ? FAST_INTERVAL : alternateDelay)) {
            previousMillis = currentMillis;

            // Turn off all LEDs
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_YELLOW_PIN, LOW);
            digitalWrite(LED_RED_PIN, LOW);
            
            // Turn on LED based on state
            switch (ledState) {
                case 0:
                    if (ledKamar) digitalWrite(LED_GREEN_PIN, HIGH);
                    break;
                case 1:
                    if (ledKamarMandi) digitalWrite(LED_YELLOW_PIN, HIGH);
                    break;
                case 2:
                    if (ledRuangTamu) digitalWrite(LED_RED_PIN, HIGH);
                    break;
            }
            ledState = (ledState + 1) % 3;
        }
    } else if (ledEnabled && (fastBlink || alternateMode)) {
        // Normal mode: LEDs blink based on state
        if (currentMillis - previousMillis >= interval) {
            ledState = !ledState;
            digitalWrite(LED_GREEN_PIN, (ledState && ledKamar) ? HIGH : LOW);
            digitalWrite(LED_YELLOW_PIN, (ledState && ledKamarMandi) ? HIGH : LOW);
            digitalWrite(LED_RED_PIN, (ledState && ledRuangTamu) ? HIGH : LOW);
            previousMillis = currentMillis;
        }
    } else if (ledEnabled) {
        // LEDs are enabled but not blinking
        digitalWrite(LED_GREEN_PIN, ledKamar ? HIGH : LOW);
        digitalWrite(LED_YELLOW_PIN, ledKamarMandi ? HIGH : LOW);
        digitalWrite(LED_RED_PIN, ledRuangTamu ? HIGH : LOW);
    } else {
        // LEDs are disabled
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_YELLOW_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_YELLOW_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);

    WiFiManager wifiManager;
    wifiManager.autoConnect("ESP8266-Setup");

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Gagal terhubung ke WiFi. Memulai ulang dalam 5 detik...");
        delay(5000);
        ESP.restart();
    }

    if (MDNS.begin("esp8266")) {
        Serial.println("mDNS responder started. You can access it at http://esp8266.local");
    } else {
        Serial.println("Gagal memulai mDNS responder. Memulai ulang dalam 5 detik...");
        delay(5000);
        ESP.restart();
    }

    setupRoutes();
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();  // Handle web server clients
    updateLEDs();          // Update LED states
}