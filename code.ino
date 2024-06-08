#include <WiFi.h>  
#include <PubSubClient.h>
#include <ArduinoJson.h>

int pinLDR = 34;  
int trigger = 25;
int echo = 26;

const char* ssid = "Wokwi-GUEST"; 
const char* password = "";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi() {  
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() { 
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }}
}

long readDistanceCM() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  long duration = pulseIn(echo, HIGH);
  return duration / 58;
}



void setup() {
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);   
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
 
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) { 
    lastMsg = now;
    Serial.print(analogRead(34));
    int light = map(analogRead(34),4063,32,0,100);
    long distance = readDistanceCM();
    

    StaticJsonDocument<200> doc;
    doc["claridade"] = String(light);
    doc["distancia"] = String(distance);

  
    String payload;
    serializeJson(doc, payload);
    client.publish("/Teste_gs/Fiap", payload.c_str());  
 
  }

}