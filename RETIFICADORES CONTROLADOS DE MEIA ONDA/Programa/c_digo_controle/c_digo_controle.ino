/*************************************************************                                                         
 *     __ __     _   _           _____     _               
 *    |  |  |_ _| |_|_|   ___   |     |___| |_ ___ _ _ ___ 
 *    |_   _| | | '_| |  | -_|  | | | | -_|  _| -_| | |_ -|
 *      |_| |___|_,_|_|  |___|  |_|_|_|___|_| |___|___|___|
 *                                                         
 *                                                         
 *  IFSC - Câmpus Florianópolis
 *  Eletrônica de Potência 2019-1                                                       
 *  Programa para o laboratório 24 - Controle de ângulo de fase
 *  
 *  Feito por: Lucas Yuki Imamura e Mateus Israel Silva
 *************************************************************/

/*************************************************************
 *  PARÂMETROS:                                                   *
 *************************************************************/
  //DEFINA O ÂNGULO DE ATIVAÇÃO

 #define  ANGULO_DE_ATIVACAO 30;  //Por padrão vem em 30, substitua o valor se quiser modificar



unsigned long t; //tempo de execução do programa
float freq; //frequência do sinal
float tim; //tempo do período
float T1; //tempo desligado do ciclo
float T2; //tempo desligado do ciclo (ajuste fino)
float T3; //tempo ligado do ciclo
float T4; //tempo ligado do ciclo (ajuste fino)


float ang = ANGULO_DE_ATIVACAO; //angulo de ativação
int Periodos = 30; //quantidade de períodos analisados para determinar a frequência
volatile bool state = HIGH;

#define Entrada 3
#define Saida 13





void setup() {
  pinMode(Saida, OUTPUT);
  digitalWrite(Saida, LOW);
  pinMode(Entrada, INPUT_PULLUP);

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 10000;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  
  Serial.begin(9600);
  
  attachInterrupt(digitalPinToInterrupt(Entrada), testar, FALLING);
  sei();
}


void loop(){
  digitalWrite(Saida, state);
}


void testar(){
  TCCR1B |= (1 << WGM12);
}

ISR(TIMER1_COMPA_vect){
  state = !state;
  TCCR1B &= (0 << WGM12);
}
