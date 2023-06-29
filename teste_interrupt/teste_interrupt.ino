const int ttp223Pin = 4;
const int LED = 2; 

int temp = 0;
void IRAM_ATTR handleTouch() {
  int ledState = digitalRead(LED);
  if (ledState == HIGH){
    digitalWrite(LED, LOW);
  }
  else{
    digitalWrite(LED, HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode (LED, OUTPUT); 
  pinMode(ttp223Pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ttp223Pin), handleTouch, RISING);
}

void loop() {
  Serial.println(temp);
  // Loop vazio, as ações são realizadas na interrupção
}
