#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Ticker.h>
#include <WiFi.h>
#include <WiFiUdp.h>


#define DHTPIN 13     // Pino de dados do sensor DHT11
#define DHTTYPE DHT11   // Tipo do sensor (DHT11)
#define PWMPIN 12
#define UPPIN 4
#define LOWPIN 2
#define MENUPIN 15
#define OLED_RESET -1
#define RELE 26


const int freq = 3;
const int resolution = 16;
const int channel = 0;
bool actual = true;
float set_temp = 20.0;
float y1_controller = 0.0;
int countSafe = 0;

Adafruit_SSD1306 display(OLED_RESET);
Ticker controller_timer;
Ticker sensor_timer;
// Ticker sendSerial;
DHTesp dht;
float actual_temp = 0;
// float setpoint = 18.0
int duty = 0;
int pwm = 0;
WiFiUDP udp;


int sendPWM(int dutyCycle){
  return 65535 - ((65535*dutyCycle)/100);
}

void sendToPython(float sensor, float setpoint, int duty, float y_controlador, int pwm){
  Serial.print(sensor);
  Serial.print(",");
  Serial.print(setpoint);
  Serial.print(",");
  Serial.print(duty);
  Serial.print(",");
  Serial.print(y_controlador);
  Serial.print(",");
  Serial.println(pwm);
}

void controller(){

  const float b0 = 1.1142, b1 = -1.11353148, b2 = 0.0;
  const float a1 = 1.0, a2 = 0.0;
  // float ref = 5;
  static float ek1 = 0 ,ek2 = 0;
  static float  uk1=0, uk2=0;

  float ek0 = set_temp - actual_temp;
  float uk0 = a1*uk1 + a2*uk2 + b0*ek0 + b1*ek1 + b2*ek2;
  uk2 = uk1;
  uk1 = uk0;
  ek2 = ek1;
  ek1= ek0; 
  float y1 = -uk0;
  y1_controller = y1;
  if (y1 > 2.0){
    y1 = 2.0;
  }
  if(y1 < 0.5){
    duty = 20;
  }
  
  else{
    duty = map(y1 * 10, 1, 20, 1, 99);
    
  }

  pwm = sendPWM(duty);
  ledcWrite(channel, pwm);
  
}





void read_sensor(){

  float temp = dht.getTemperature();
  if (isnan(temp) || (temp == 0.0)){
    countSafe += 1;
    actual_temp = actual_temp;
  }else{
    actual_temp = temp;
    countSafe == 0;
    digitalWrite(RELE, LOW);
  }
  sendToPython(actual_temp, set_temp, duty, y1_controller, pwm);
    
}




void IRAM_ATTR up() {
  if(set_temp < 30.0 && actual == false){
    set_temp += 0.5;
  } 
}
void IRAM_ATTR low(){
  if(set_temp > 8.0 && actual == false){
  set_temp -= 0.5;
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
void setup() {
  Serial.begin(9600);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(PWMPIN, channel);
  dht.setup(DHTPIN, DHTesp::DHT11);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  // pinMode(DHT_PIN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(DHTPIN), readSensor, RISING);
  controller_timer.attach(2, controller);  
  sensor_timer.attach(6, read_sensor);
  pinMode(RELE, OUTPUT);
  pinMode(UPPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UPPIN), up, RISING);
  pinMode(LOWPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LOWPIN), low, RISING);
  pinMode(MENUPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MENUPIN), menu, RISING);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED){
  //   delay(1000);
  //   Serial.println("Conectando ao WiFi...");
  // }
  // Serial.println("Conectado ao WiFi");

  // udp.begin(udpServerPort);
}


void loop() {
  // Serial.println("aloooo");
  // verifyCountSafe();
  delay(10);  // Intervalo de leitura
  display.clearDisplay();

  display.setTextSize(1.3); // Define o tamanho do texto
  display.setRotation(2);
  display.setTextColor(SSD1306_WHITE); // Define a cor do texto (branco)
 
  if (actual == false){
    display.setCursor(30, 0);
    display.println("Setpoint: ");
    display.setTextSize(2);
    display.setCursor(30, 0);
    display.setCursor(30, 13);
    display.println(String(set_temp));
  }
  else{
    display.setTextSize(1);
    display.setCursor(30, 0);
    display.println("Temperatura: ");
    display.setTextSize(2);
    display.setCursor(30, 13);
    display.println(String(actual_temp));
    
  } 
  display.setCursor(100, 13);
  display.print("C");
  // Define a posição do cursor // Imprime a primeira linha
  // display.println("World!"); // Imprime a segunda linha


  display.display(); // Exibe o texto no display

  

}
