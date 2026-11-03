#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

// =========================================================================
// CONFIGURAÇÕES WI-FI: Substitua pelos dados da sua rede (2.4 GHz)
// =========================================================================
const char *ssid = "Iphone de Santana"; // Nome da rede Wi-Fi
const char *password = "lucaslindo20";  // Senha da rede Wi-Fi

ESP8266WebServer server(80);

// LED interno do NodeMCU (geralmente GPIO2 = D4 no ESP-12E / NodeMCU)
const int ledPin = LED_BUILTIN;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html +=
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Controle do LED</title></head>";
  html += "<body style='text-align:center; font-family:Arial, sans-serif; "
          "margin-top:50px;'>";
  html += "<h1>Controle do LED interno</h1>";
  html += "<p><a href='/on'><button style='font-size:20px; padding:12px 28px; "
          "margin:10px; cursor:pointer;'>Ligar</button></a></p>";
  html += "<p><a href='/off'><button style='font-size:20px; padding:12px 28px; "
          "margin:10px; cursor:pointer;'>Desligar</button></a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(ledPin,
               LOW); // LOW liga o LED interno (lógica invertida no NodeMCU)
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  digitalWrite(ledPin, HIGH); // HIGH desliga o LED interno
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Começa desligado

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
  Serial.println("Servidor web iniciado!");
}

void loop() { server.handleClient(); }
