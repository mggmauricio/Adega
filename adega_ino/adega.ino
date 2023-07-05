#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);
// #include <heltec.h>

#define LMPIN A0


const int freq = 3;
const int resolution = 16;
const int channel = 0;
float set_temp = 20.0;
bool actual = true;

DHTesp dht;

void IRAM_ATTR up() {
  if(set_temp <= 23.0 && actual == false){
    set_temp += 0.1;
  } 
}
void IRAM_ATTR low(){
  if(set_temp >= 12.0 && actual == false){
  set_temp -= 0.1;
  }
}

void IRAM_ATTR menu(){
  if (actual == false){
    actual = true;
  }
  else{
    actual = false;
  }
}
int sendPWM(int dutyCycle){
  return 65535-((65535*dutyCycle)/100);
}

void setup() {
  Serial.begin(9600);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(PWMPIN, channel);
  dht.setup(DHTPIN, DHTesp::DHT11);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(UPPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UPPIN), up, RISING);
  pinMode(LOWPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LOWPIN), low, RISING);
  pinMode(MENUPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MENUPIN), menu, RISING);
}

void loop() {
  float temperature = dht.getTemperature();
  delay(2000);  // Intervalo de leitura
  display.clearDisplay();

  display.setTextSize(2); // Define o tamanho do texto
  display.setTextColor(SSD1306_WHITE); // Define a cor do texto (branco)
  display.setCursor(0, 0);
  if (actual == false){
    display.println(String(set_temp));
  }
  else{
    display.println(String(temperature));
  } // Define a posição do cursor // Imprime a primeira linha
  // display.println("World!"); // Imprime a segunda linha

  display.display(); // Exibe o texto no display

  if (isnan(temperature)) {
    Serial.println('0');
  } else {
    Serial.println(String(temperature));
  }
 int pwm = sendPWM(90);
  ledcWrite(channel, pwm);
  float humidity = dht.getHumidity();

  

}
