#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "goteborgfree";  // Replace with your Wi-Fi SSID
const char* password = "";                    // No password for open networks

WebServer server(80); // Web server on port 80

void handleRoot() {
    server.send(200, "text/plain", "Hello, ESP32 Web Server!");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up web server routes
    server.on("/", handleRoot);

    server.begin();
    Serial.println("Web server started!");
}

void loop() {
    server.handleClient(); // Handle incoming client requests
}
