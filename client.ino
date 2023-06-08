#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>

const char* ssid = "scd";
const char* password = "12345678";
const int numRequests = 23;

WebServer server(80);

void handleMain() {
  server.send(200, "text/plain", "hello from esp32!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRoot() {
  int paramValue = server.arg("param").toInt();
  HTTPClient httpClient;
  // Construir la URL del endpoint /download con el parámetro "param"
  String url = "http://192.168.4.1/bit?param=" + String(paramValue);
  httpClient.begin(url);
  int httpResponseCode = httpClient.GET();
  Serial.println(httpResponseCode);
  String response = httpClient.getString();
  server.sendHeader("Content-Disposition", "attachment; filename=\"output" + String(paramValue) + ".txt\"");
  server.send(200, "application/octet-stream", response);
}

void setup() {
  Serial.begin(115200);

  // Conectar a la red WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }

  Serial.println("Conexión WiFi establecida");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Configurar el servidor web
  server.on("/",[]() {
    Serial.print("empty endpoint: ");
    server.send(200, "text/plain", "SCD ALEJANDRO VARELA ");
  });

  server.on("/download", handleRoot);

  server.onNotFound(handleNotFound);

  // Iniciar el servidor web
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
