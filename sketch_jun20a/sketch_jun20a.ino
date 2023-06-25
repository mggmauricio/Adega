#include <DHTesp.h>
#include <heltec.h>

#define DHTPIN 17     // Pino de dados do sensor DHT11
#define DHTTYPE DHT11   // Tipo do sensor (DHT11)
#define PWMPIN 13


const int freq = 3;
const int resolution = 16;
const int channel = 0;


DHTesp dht;
int sendPWM(int dutyCycle){
  return 65535-((65535*dutyCycle)/100);
}

void setup() {
  Serial.begin(9600);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(PWMPIN, channel);
  dht.setup(DHTPIN, DHTesp::DHT11);
  // Serial.println("Inicialização completa");
  // Heltec.begin(true, true, true, true, 11520);
  Heltec.display->init();
  // Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Adega");
  Heltec.display->drawString(10, 30, "inicializada!");
  Heltec.display->display();
  delay(1000);
}

void loop() {
  // delay(2000);  // Intervalo de leitura

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
    Serial.println('0');
  } else {
    Serial.println(String(temperature));
  }
  int pwm = sendPWM(80);
  ledcWrite(channel, pwm);
  float humidity = dht.getHumidity();
  // if (isnan(humidity)) {
  //   Serial.println("Erro ao ler a umidade!");
  // } else {
  //   Serial.print("Umidade: ");
  //   Serial.print(humidity);
  //   Serial.println("%");
  // }
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(20, 0, "Temperatura");
  Heltec.display->drawString(50, 15, "atual:");
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(13, 35, String(temperature));
  Heltec.display->drawString(78, 35, "°C");
  Heltec.display->display();
  delay(2000);
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  

}
