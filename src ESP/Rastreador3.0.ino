//Importando bibliotecas necessárias
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "UbidotsEsp32Mqtt.h"

//Variáveis usadas para mostrar mensagens na tela LCD
int lcdCol = 16;
int lcdLin = 2;
String idRast = "Rastreador: 1";
LiquidCrystal_I2C lcd(0x27, lcdCol, lcdLin); 

//Variaveis usadas na configuração e envio para o Ubidots
unsigned long timerRef = 0;
int delta = 5000;
String bssid;
const char *token = "BBFF-N3cchS1UxN9IV9BiW89vxaok8RFdSQ";
const char *ssid = "Inteli-COLLEGE";
const char *senha = "QazWsx@123";
const char *rot = "Rastreador1";
const char *var = "Potencia";
char *rede = "MAC do roteador";
const char* on = "Removido";
int rssi;
Ubidots ubidots(token);

//Função chamada ao enviar uma mensagem para o Ubidots
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

  //Inicializando o pin do LDR
  pinMode(32,INPUT);

  //Inicializando a tela LCD
  lcd.init();                   
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(idRast);
  
  //Configurando o Ubidots
  ubidots.connectToWifi(ssid, senha);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(rot, var);
}

//Função que interpreta a leitura do LDR para saber se o rastreador foi removido ou não da case
int isRemoved(){
  int value = analogRead(32);
  Serial.println(value);
  if(value>500){
    return 0;
  } else {
    return 1;
  }
}

//Função que pega os últimos 4 dígitos do endereço MAC do rastreador e transforma em um número decimal
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

//Função que muda o intervalo da leitura do RSSI
float interval(int a){
  float c = (float) a;
  int b = 100 + c/90*100;
  return b;
}

void loop() {
  //Loop para reconectar a internet caso necessário
  if (!ubidots.connected()) {
    ubidots.reconnect();
    Serial.println("Reconecting...");
  }

  //Variáveis que precisam ser atulizadas rapidamente
  rssi = (int) WiFi.RSSI();
  bssid = WiFi.BSSIDstr();

  //Loop que permite o envio de informações depois de um período de tempo tanto para o Ubidots quanto para o LCD
  if(millis()-timerRef >= delta){
    int hexa = hexToDec();
    timerRef = millis();
    String message = "Final MAC: " + String(bssid[12])+String(bssid[13])+":"+String(bssid[15])+String(bssid[16]);
    ubidots.add(var, interval(rssi));
    ubidots.publish(rot);
    ubidots.add(rede, hexa);
    ubidots.publish(rot);
    ubidots.add(on,isRemoved());
    ubidots.publish(rot);
    lcdDisplay(message);
    Serial.println("Message sent");
  }
  //Envio da informação
  ubidots.loop();
}