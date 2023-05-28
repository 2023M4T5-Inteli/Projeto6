//Importando bibliotecas necessárias
#include <esp_now.h>
#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"

//Constantes para a conexão com o Ubidots
const char *token = "BBFF-N3cchS1UxN9IV9BiW89vxaok8RFdSQ";
//const char *ssid = "Inteli-COLLEGE";
//const char *senha = "QazWsx@123";
const char *ssid = "SHARE-RESIDENTE";
const char *senha = "Share@residente23";
const char *rot = "Roteador1";
const char *ras = "Rastreador";
const char *tmp = "Clock do Rastreador";
Ubidots ubidots(token);

//Variáveis necessárias
int macRas = 0;
int clkRas = 0;
int macRot = 14180;
const int frequencia = 5000;
unsigned long timer;

//Definição do pacote que será recebido
struct packet {
  int macRast;
  int clock;
};

packet myData;


//Função que retorna o que foi enviado ao Ubidots
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print(topic);
  Serial.print("]");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//Função que recebe os dados do rastreador pelo ESP-NOW
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len){
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes recebidos: ");
  Serial.print("Endereço MAC (4 últimos dígitos em decimal): ");
  Serial.println(myData.macRast);
  macRas = myData.macRast;
  Serial.print("Clock do rastreador: ");
  Serial.println(myData.clock);
  clkRas = myData.clock;
  Serial.println();
}


void setup(){
  //Iniliza o monitor serial
  Serial.begin(9600);

  //Declaração do led no sistema físico
  pinMode(33,OUTPUT);

  //Conexão com o wifi e inicialização do ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Setup para a conectividade com o Ubidots
  ubidots.connectToWifi(ssid, senha);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(rot, ras);

  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop(){
  //Código que acende o led se o roteador está conectado com o dashboard do Ubidots
  if (!ubidots.connected()) {
    ubidots.reconnect();
    digitalWrite(33,LOW);
  } else {
    digitalWrite(33,HIGH);
  }
  
  ////Loop que permite o envio de informações depois de um determinado período de tempo
  if (millis() - timer > frequencia){
    ubidots.add(ras, macRas);
    ubidots.publish(rot);
    ubidots.add(tmp,clkRas);
    ubidots.publish(rot);
    timer = millis();
  }
  ubidots.loop();
}