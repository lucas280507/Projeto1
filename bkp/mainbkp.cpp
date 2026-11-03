#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// =========================================================================
// CONFIGURAÇÕES WI-FI: Substitua pelos dados da sua rede (2.4 GHz)
// =========================================================================
const char* ssid = "S26 Ultra de Heitor";       // Nome da rede Wi-Fi
const char* password = "heitor12"; // Senha da rede Wi-Fi

ESP8266WebServer server(80);

// LED interno do NodeMCU (geralmente GPIO2 = D4 no ESP-12E / NodeMCU)
const int ledPin = LED_BUILTIN;
bool ledState = false; // false = desligado, true = ligado

// Template HTML + CSS moderno salvo na memória Flash (PROGMEM)
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Controle do NodeMCU - LED</title>
  <style>
    :root {
      --bg: #0f172a;
      --card-bg: rgba(30, 41, 59, 0.75);
      --card-border: rgba(255, 255, 255, 0.08);
      --text-main: #f8fafc;
      --text-sub: #94a3b8;
      --btn-on-bg: linear-gradient(135deg, #10b981 0%, #059669 100%);
      --btn-on-glow: rgba(16, 185, 129, 0.35);
      --btn-off-bg: linear-gradient(135deg, #ef4444 0%, #b91c1c 100%);
      --btn-off-glow: rgba(239, 68, 68, 0.35);
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body {
      background-color: var(--bg);
      background-image: 
        radial-gradient(at 0% 0%, rgba(56, 189, 248, 0.12) 0px, transparent 50%),
        radial-gradient(at 100% 100%, rgba(99, 102, 241, 0.12) 0px, transparent 50%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
      color: var(--text-main);
    }
    .card {
      background: var(--card-bg);
      backdrop-filter: blur(16px);
      -webkit-backdrop-filter: blur(16px);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      padding: 40px 32px;
      width: 100%;
      max-width: 420px;
      box-shadow: 0 20px 40px -15px rgba(0, 0, 0, 0.5);
      text-align: center;
    }
    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--card-border);
      padding: 6px 14px;
      border-radius: 100px;
      font-size: 13px;
      color: #38bdf8;
      font-weight: 600;
      margin-bottom: 24px;
      letter-spacing: 0.5px;
      text-transform: uppercase;
    }
    .badge-dot {
      width: 8px; height: 8px; border-radius: 50%;
      background: #38bdf8; box-shadow: 0 0 10px #38bdf8;
    }
    h1 { font-size: 26px; font-weight: 700; margin-bottom: 8px; letter-spacing: -0.5px; }
    p.subtitle { color: var(--text-sub); font-size: 14px; margin-bottom: 32px; }
    .led-display {
      margin: 0 auto 32px auto;
      width: 90px; height: 90px; border-radius: 50%;
      background: #1e293b;
      border: 3px solid rgba(255, 255, 255, 0.1);
      display: flex; align-items: center; justify-content: center;
      transition: all 0.4s ease;
    }
    .led-display svg { width: 42px; height: 42px; fill: #64748b; transition: all 0.4s ease; }
    .led-display.active {
      background: rgba(16, 185, 129, 0.15);
      border-color: #10b981;
      box-shadow: 0 0 35px rgba(16, 185, 129, 0.45);
    }
    .led-display.active svg { fill: #10b981; filter: drop-shadow(0 0 8px #10b981); }
    .btn-group { display: flex; flex-direction: column; gap: 14px; }
    .btn {
      display: flex; align-items: center; justify-content: center; gap: 10px;
      width: 100%; padding: 16px 24px; font-size: 16px; font-weight: 600;
      border-radius: 14px; border: none; cursor: pointer; text-decoration: none;
      transition: all 0.25s cubic-bezier(0.4, 0, 0.2, 1);
      color: #ffffff; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.25);
    }
    .btn:active { transform: scale(0.97); }
    .btn-on { background: var(--btn-on-bg); box-shadow: 0 8px 20px var(--btn-on-glow); }
    .btn-on:hover { box-shadow: 0 10px 25px rgba(16, 185, 129, 0.55); transform: translateY(-2px); }
    .btn-off { background: var(--btn-off-bg); box-shadow: 0 8px 20px var(--btn-off-glow); }
    .btn-off:hover { box-shadow: 0 10px 25px rgba(239, 68, 68, 0.55); transform: translateY(-2px); }
    footer { margin-top: 32px; font-size: 12px; color: var(--text-sub); }
  </style>
</head>
<body>
  <div class="card">
    <div class="badge"><span class="badge-dot"></span>ESP8266 • NodeMCU</div>
    <h1>Controle do LED</h1>
    <p class="subtitle">Projeto Integrador • Servidor Web Local</p>
    <div class="led-display %LED_CLASS%" id="ledStatus">
      <svg viewBox="0 0 24 24">
        <path d="M12 2C8.13 2 5 5.13 5 9c0 2.38 1.19 4.47 3 5.74V17c0 .55.45 1 1 1h6c.55 0 1-.45 1-1v-2.26c1.81-1.27 3-3.36 3-5.74 0-3.87-3.13-7-7-7zm-3 18c0 .55.45 1 1 1h4c.55 0 1-.45 1-1v-1H9v1z"/>
      </svg>
    </div>
    <div class="btn-group">
      <a href="/on" class="btn btn-on">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="currentColor">
          <path d="M13 3h-2v10h2V3zm4.83 2.17l-1.42 1.42C17.99 7.86 19 9.81 19 12c0 3.87-3.13 7-7 7s-7-3.13-7-7c0-2.19 1.01-4.14 2.58-5.42L6.17 5.17C4.23 6.82 3 9.26 3 12c0 4.97 4.03 9 9 9s9-4.03 9-9c0-2.74-1.23-5.18-3.17-6.83z"/>
        </svg>
        Ligar LED
      </a>
      <a href="/off" class="btn btn-off">
        <svg width="20" height="20" viewBox="0 0 24 24" fill="currentColor">
          <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8zm4-9H8v2h8v-2z"/>
        </svg>
        Desligar LED
      </a>
    </div>
    <footer>Conexão direta via Wi-Fi</footer>
  </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  String html = INDEX_HTML;
  html.replace("%LED_CLASS%", ledState ? "active" : "");
  server.send(200, "text/html", html);
}

void handleOn() {
  ledState = true;
  digitalWrite(ledPin, LOW); // LOW liga o LED interno (lógica invertida no NodeMCU)
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  ledState = false;
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

void loop() {
  server.handleClient();
}
