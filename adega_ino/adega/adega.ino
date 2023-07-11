#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Ticker.h>

#define DHTPIN 13     // Pino de dados do sensor DHT11
#define DHTTYPE DHT11   // Tipo do sensor (DHT11)
#define PWMPIN 12
#define UPPIN 4
#define LOWPIN 2
#define MENUPIN 15
#define OKPIN 17
#define OLED_RESET -1



const int freq = 3;
const int resolution = 16;
const int channel = 0;
bool actual = true;
float set_temp = 20.0;
Adafruit_SSD1306 display(OLED_RESET);
Ticker timer;
DHTesp dht;
float temperature = 0;


int sendPWM(int dutyCycle){
  return 65535-((65535*dutyCycle)/100);
}


int controller(float sensor, float setpoint){
  float temperature_sensor = sensor;
  int duty = 0;
  float setpoint_ = setpoint;
  const float b0 = 0.6024, b1 = -1.19329416, b2 = 0.590917;
  const float a1 = 1.9581, a2 = -0.9581;
  // float ref = 5;
  static float ek1 = 0 ,ek2 = 0;
  static float  uk1=0, uk2=0;
  float ek0 = setpoint_ - temperature_sensor;
  float uk0 = a1*uk1 + a2*uk2 + b0*ek0 + b1*ek1 + b2*ek2;
  uk2 = uk1;
  uk1 = uk0;
  ek2 = ek1;
  ek1= ek0; 
  float y1 = -uk0;
  if (y1 <= 0.0){
   duty = 0; 
  } else if (y1 >= 4.0){
    duty = 100;
  } else {
    duty = map(y1 * 10, 1, 79, 1, 99);
  }

  Serial.print("Saída do controlador: ");
  Serial.println(y1);
  Serial.print("Duty Cycle: ");
  Serial.println(duty);
  return duty;
  // return y1
}





void read_sensor(){
  int pwm_percent = 0;
  int Pwm = 0;
  float temp = dht.getTemperature();
  if (isnan(temp)){
  }else{
    temperature = temp;
    pwm_percent = controller(temperature, set_temp);
    Pwm = sendPWM(pwm_percent);
    ledcWrite(channel, Pwm);
    Serial.print("Pwm: ");
    Serial.println(Pwm);
  }

}




void IRAM_ATTR up() {
  if(set_temp < 30.0 && actual == false){
    set_temp += 0.5;
  } 
}
void IRAM_ATTR low(){
  if(set_temp > 12.0 && actual == false){
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
  timer.attach(3, read_sensor);
  pinMode(UPPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UPPIN), up, RISING);
  pinMode(LOWPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LOWPIN), low, RISING);
  pinMode(MENUPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MENUPIN), menu, RISING);
}

void loop() {
  
  delay(10);  // Intervalo de leitura
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

  

}
