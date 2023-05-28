//Importando bibliotecas necessárias
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>

//Variáveis usadas para mostrar mensagens na tela LCD
int lcdCol = 16;
int lcdLin = 2;
String idRast = "Rastreador: 1";
LiquidCrystal_I2C lcd(0x27, lcdCol, lcdLin); 
unsigned long timerRef = 0;
int delta = 10000;

//Endereço Mac dos roteadores
uint8_t broadcastAddress1[] = {0xE0, 0x5A, 0x1B, 0x6C, 0x11, 0x80};
uint8_t broadcastAddress2[] = {0x40, 0x22, 0xD8, 0x03, 0x37, 0x64};

//Definição do pacote a ser enviado para os roteadores
struct packet {
  int macRast;
  int clock;
}; 

//Instanciamento do struct e variáveis para o ESP-NOW
struct packet info1;
struct packet info2;
long timer;
esp_now_peer_info_t peerInfo;

//Função que será chamada ao enviar a informação para os roteadores
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if(status==ESP_NOW_SEND_SUCCESS){
    lcdDisplay("Delivery Success");
  } else {
    lcdDisplay("Delivery Fail");
  }
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
  //Inicializando a tela LCD
  lcd.init();                   
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(idRast);
  //Inicializando o monitor serial
  Serial.begin(115200);

  //Configurações para o uso do ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
   
  //Registrando os roteadores para enviar os pacotes
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  //Loop que permite o envio de informações depois de um período de tempo
  if(millis()-timerRef >= delta){

    //Associação de variáveis para o envio
    timer = millis();
    timerRef = millis();
    info1.macRast = 4480;
    info1.clock = (int) timer%180000;

    //Envio da informação
    esp_err_t result = esp_now_send(broadcastAddress2, (uint8_t *) &info1, sizeof(packet));
  }
}