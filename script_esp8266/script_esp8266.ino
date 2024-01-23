#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <MFRC522.h>


#define SS_PIN  15  
#define RST_PIN 0
#define BUZZER_PIN 5

const char* ssid     = "OnePlus 10T";
const char* password = "123456789";
const char *mqtt_server = "192.168.70.57";
const char *mqtt_topic_code = "code";
const char *mqtt_topic_error = "error";
String tagID = "";

char msgADD[200];
StaticJsonDocument<200> jsonDocument; // json per i dati da aggiungere al DB

// Index html per la pagina di add e delete
const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML><html><head>
    <title>ESP Input Form</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    </head><body>
    <form action="/add">
        ID: <input type="text" name="id">
        Nome: <input type="text" name="name">
        Cognome: <input type="text" name="surname">
        Ruolo: <input type="text" name="role">
        <input type="submit" value="Aggiungi">
    </form>
    <div></div>
    <form action="/delete">
        <input type="submit" value="Elimina">
    </form>
    </body></html>)rawliteral";

MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);

void callback(char* topic, byte* payload, unsigned int length) { // funzione di callback per il messaggio di errore
  Serial.print("Messaggio ricevuto su topic: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);
  tone(BUZZER_PIN, 50, 500);
   
}

void handleHome() { // home che chiama index_html
  server.send(200, "text/html", index_html);
}

void handleDelete() { // funzione per mandare il comando delete
  client.publish("delete", "delete");
  tone(BUZZER_PIN, 600, 100); // idea iniziale di mettere command come topic
}

void handleAdd() {// funzione per mandare il comando add con dati annessi
  if(server.arg("id") != "" && server.arg("name") != "" && server.arg("surname") != "" && server.arg("role") != "") {
    tone(BUZZER_PIN, 600, 100);

    // Creazione Json
    jsonDocument["id"] = server.arg("id");
    jsonDocument["nome"] = server.arg("name");
    jsonDocument["cognome"] = server.arg("surname");
    jsonDocument["ruolo"] = server.arg("role");

    serializeJson(jsonDocument, msgADD); // Creazione stringa json
    Serial.println(msgADD);
    client.publish("add", msgADD);

    server.send(200, "text/html", "Informazioni Dipendente inviate con successo!");

    delay(1000);

  } else {
    server.send(200, "text/plain", "NO CREDENTIAL FOUND");
    tone(BUZZER_PIN, 50, 400);
  }
}

void setupRouting() {
  server.on("/", handleHome);
  server.on("/delete", handleDelete);
  server.on("/add", handleAdd);
  server.begin();
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  delay(1000);

  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tone(BUZZER_PIN, 100, 200);
  }
  Serial.println(" ");
  Serial.print("Ip: ");
  Serial.println(WiFi.localIP());

  if(WiFi.isConnected()) {
    tone(BUZZER_PIN, 600, 100);
    SPI.begin();      // Inizializza la comunicazione SPI
    mfrc522.PCD_Init();  // Inizializza il modulo RFID-RC522
    client.setServer(mqtt_server, 1883);
    setupRouting();
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  
  if(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    saveTagID(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println(tagID);
    client.publish(mqtt_topic_code, tagID.c_str());
    tone(BUZZER_PIN, 800, 500);
    delay(200);
  }

  server.handleClient();
  client.loop();
  delay(500);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe(mqtt_topic_error);
      client.setCallback(callback);
      Serial.println("Connesso al MQTT broker");
    } else {
      tone(BUZZER_PIN, 60, 200);
      Serial.print("Connessione fallita ");
      Serial.println(client.state());
      Serial.println("Riprova tra 3 secondi");
      delay(3000);
    }
  }
}

void saveTagID(byte *uid, byte bufferSize) {
  tagID = ""; // così rimane vuota ogni volta che viene chiama la funzione saveTagID
  for (byte i = 0; i < bufferSize; i++) {
    // Concatena l'ID del tag nella variabile tagId
    tagID += String(uid[i], HEX);
  }
}