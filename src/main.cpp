#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ----------------------------------------------------------------------
// CONFIGURATION
// ----------------------------------------------------------------------
// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// GPIO Pin connected to the Relay / Coin mechanism
// Ensure you are using a pin safe for output (e.g., 5, 4, 13, etc.)
// AVOID: 6-11 (connected to flash), 34-39 (input only)
const int COIN_PIN = 5; 

// Duration of the "coin inserted" signal in milliseconds
// Standard coin mechs might need 50-100ms. Adjust as needed.
const int COIN_PULSE_DURATION_MS = 100;

// Logic Level for "Active" (Coin inserted)
// If you are using a Relay Module:
//   - Most are "Active LOW" (Set LOW to turn ON) -> set true if so
//   - If Active HIGH -> set false
const bool RELAY_ACTIVE_LOW = true; 

// ----------------------------------------------------------------------
// GLOBALS
// ----------------------------------------------------------------------
WebServer server(80);

// ----------------------------------------------------------------------
// HELPER FUNCTIONS
// ----------------------------------------------------------------------

void triggerCoin() {
  Serial.println("Triggering Coin...");
  
  // Determine ON and OFF states based on configuration
  int ON_STATE = RELAY_ACTIVE_LOW ? LOW : HIGH;
  int OFF_STATE = RELAY_ACTIVE_LOW ? HIGH : LOW;

  digitalWrite(COIN_PIN, ON_STATE);
  delay(COIN_PULSE_DURATION_MS);
  digitalWrite(COIN_PIN, OFF_STATE);
  
  Serial.println("Coin Triggered.");
}

// ----------------------------------------------------------------------
// HTTP HANDLERS
// ----------------------------------------------------------------------

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Pinball Controller</title>";
  html += "<style>body{font-family:sans-serif;text-align:center;padding:50px;}";
  html += "button{padding:20px;font-size:20px;cursor:pointer;background:#28a745;color:white;border:none;border-radius:5px;}";
  html += "</style></head><body>";
  html += "<h1>Pinball Coin Controller</h1>";
  html += "<p>Status: Ready</p>";
  html += "<form action=\"/coin\" method=\"GET\"><button type=\"submit\">INSERT COIN</button></form>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleCoin() {
  triggerCoin();
  // Redirect back to root or show success
  server.send(200, "text/html", "<h1>Coin Inserted!</h1><p><a href='/'>Go Back</a></p><script>setTimeout(function(){window.location.href='/';}, 2000);</script>");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

// ----------------------------------------------------------------------
// MAIN SETUP & LOOP
// ----------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\n\n--- ESP32 Pinball Controller ---");

  // Setup GPIO
  pinMode(COIN_PIN, OUTPUT);
  // Ensure it starts in OFF state
  digitalWrite(COIN_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);

  // Setup WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup mDNS
  if (MDNS.begin("pinball")) {
    Serial.println("MDNS responder started");
    Serial.println("Access via http://pinball.local");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  // Setup Server
  server.on("/", handleRoot);
  server.on("/coin", handleCoin);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2); // allow CPU to switch to other tasks if needed
}
