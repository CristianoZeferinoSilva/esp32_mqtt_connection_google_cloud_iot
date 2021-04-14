#include <Arduino.h>
#include <Client.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>

//Configuração WiFi
const char *ssid = "TP-Link_7sdeC";
const char *password = "*********";

//Configuração do projeto Google IoT Core
const char *project_id = "project_id-iot";
const char *location = "location-west1";
const char *registry_id = "registry_id";
const char *device_id = "esp32-device_id";

const char* ntp_primary = "pool.ntp.org";
const char* ntp_secondary = "time.nist.gov";

//Key privada esta é uma chave de exemplo (para extrair: openssl ec -in ficheiro_da_key.pem -noout -text
const char *private_key_str =
    "2f:6f:d7:39:71:5f:c3:28:d4:d2:97:a1:f0:12:e9:"
    "27:9a:73:3b:09:69:6e:e4:ec:c2:47:3f:f5:a7:01:"
    "19:6b";
    
    
    uint32_t lastMillis = 0;

//Tempo para o JWT expirar
const int jwt_exp_secs = 60*20;

Client *netClient;
CloudIoTCoreDevice *device;
CloudIoTCoreMqtt *mqtt;
MQTTClient *mqttClient;
unsigned long iat = 0;
String jwt;

void messageReceived(String &topic, String &received_data){
  Serial.println("Mensagem recebida: " + received_data + "-> Topico: [" + topic + "]"); 
}

void sendData(String topic, String data){
  mqtt->publishTelemetry(topic, data);
  Serial.println("Mensagem enviada: " + data + " -> Tópico: [" + topic + "]");
}

//Gerar JWT para a autenticação
String getJwt(){
  iat = time(nullptr);
  Serial.println("Refreshing JWT");
  jwt = device->createJWT(iat, jwt_exp_secs);
  return jwt;
}

void setupWifi(){
  Serial.println("A conectar ao Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  configTime(0, 0, ntp_primary, ntp_secondary);
  Serial.println("Waiting on time sync...");
  while (time(nullptr) < 1510644967){
    delay(50);
  }
}

//Conexão com o servidor MQTT IoT Core
void setupCloudIoT(){
  device = new CloudIoTCoreDevice(project_id, location, registry_id, device_id, private_key_str);
  setupWifi(); //Chama a função para a conexão do WiFi
  netClient = new WiFiClientSecure();
  mqttClient = new MQTTClient(512);
  mqttClient->setOptions(180, true, 1000);
  mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
  mqtt->setUseLts(true);
  mqtt->startMQTT();
}

void connect(){
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  mqtt->mqttConnect();
}

void setup() {
  Serial.begin(115200);
  setupCloudIoT(); //Função para setup da conexão com o servidor MQTT IoT Core
}

void loop() {
  mqtt->loop();

  //Caso o ESP não esteja conectado, chama a função connect
  if(!mqttClient->connected()){
    connect();
  }

  //Enviar dados a cada 3 segundos (120000ms)
  if (millis() - lastMillis > 120000) {
    lastMillis = millis();
    sendData("/topico_esp_sub", "Mensagem");
  }
  
}