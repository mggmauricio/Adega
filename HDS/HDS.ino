#define TERMISTOR A0

// Parâmetros do termistor
const double beta = 3600.0;
const double r0   = 100000.0;
const double t0   = 273.0 + 25.0;
const double rx   = r0 * exp( -beta / t0 );
 
// Parâmetros do circuito
const double vcc = 5.0;
const double R   = 10000.0;

// Numero de amostras no sistema para a média de valores
const int nAmostras = 5;


void setup() {
  Serial.begin(9600);  
}

void loop() {
  // Le o sensor algumas vezes
  int soma = 0;
  for (int i = 0; i < nAmostras; i++) {
    soma += analogRead( TERMISTOR );
    delay (10);
  }
 
  // Determina a resistência do termistor
  double v  = ( vcc*soma ) / ( nAmostras*1024.0);
  double rt = ( vcc*R ) / v - R;
 
  // Calcula a temperatura
  float temperatura = ( beta / log(rt/rx) ) - 273.0;
  
  byte *b = (byte *) &temperatura;
  for ( int i = 0; i < sizeof( float ); i++ ) {
    Serial.write( b[i] );
  }
  delay(2000);
}
