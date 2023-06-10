//Importando bibliotecas necessárias
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"

//Variáveis usadas para mostrar mensagens na tela LCD
int lcdCol = 16;
int lcdLin = 2;
String idRast = "Rastreador: 1";
LiquidCrystal_I2C lcd(0x27, lcdCol, lcdLin); 
unsigned long timerRef = 0;
int delta = 5000;
String bssid;

const char *token = "BBFF-N3cchS1UxN9IV9BiW89vxaok8RFdSQ";
//const char *ssid = "inteli";
//const char *senha = "inteli1234";
// const char *ssid = "SHARE-RESIDENTE";
// const char *senha = "Share@residente23";
const char *ssid = "Inteli-COLLEGE";
const char *senha = "QazWsx@123";
const char *rot = "Rastreador1";
const char *var = "Potencia";
char *rede = "MAC do roteador";



int rssi;

Ubidots ubidots(token);


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print(topic);
  Serial.print("]");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//Função que atualiza a tela LCD
void lcdDisplay(String state){
  lcd.setCursor(0, 1);
  lcd.print(state);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(idRast);

}


void setup() {
  //Inicializando o monitor serial
  Serial.begin(115200);

  //Inicializando a tela LCD
  lcd.init();                   
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(idRast);

  ubidots.connectToWifi(ssid, senha);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(rot, var);
  //Configurações para o uso do ESP-NOW
}

int hexToDec(){
  int number = 0;
  String hex[] = {String(bssid[12]),String(bssid[13]),String(bssid[15]),String(bssid[16])};
  int dec[] = {0,0,0,0};
  for(int i = 0; i < 4; i++){
    String a = hex[i];
    if(a=="A"){dec[i]=10;}
    else if(a=="B"){dec[i]=11;}
    else if(a=="C"){dec[i]=12;}
    else if(a=="D"){dec[i]=13;}
    else if(a=="E"){dec[i]=14;}
    else if(a=="F"){dec[i]=15;}
    else{dec[i]=a.toInt();}
    number += dec[i]*pow(16,3-i);
  }
  return number;
} 

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
    Serial.println("Reconecting...");
  }
  rssi = (int) WiFi.RSSI();
  bssid = WiFi.BSSIDstr();

  // Serial.print(bssid[16]);
  // Serial.print(bssid[15]);
  // Serial.print(bssid[13]);
  // Serial.print(bssid[12]);
  // int hex[] = {(int)bssid[12],(int)bssid[13],(int)bssid[15],(int)bssid[16]};
  
  //Loop que permite o envio de informações depois de um período de tempo
  if(millis()-timerRef >= delta){
    Serial.println(bssid);
    int hexa = hexToDec();
    timerRef = millis();
    String message = "Final MAC: " + String(bssid[12])+String(bssid[13])+":"+String(bssid[15])+String(bssid[16]);
    ubidots.add(var, rssi);
    ubidots.publish(rot);
    ubidots.add(rede, hexa);
    ubidots.publish(rot);
    lcdDisplay(message);
    Serial.println("Message sent");
  
  
  //Envio da informação
  }
  ubidots.loop();
}