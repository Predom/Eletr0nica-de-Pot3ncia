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
  pinMode(Entrada, INPUT);
  Serial.begin(9600);
  
  //attachInterrupt(digitalPinToInterrupt(Entrada), testar, FALLING);
  
  while (digitalRead(Entrada)){}  //trava o laço enquanto a entrada for 1
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0
  t = micros(); //t0
  for (int p=0; p<Periodos-1; p++) //espera Períodos-1 ciclos  
  {
    while (digitalRead(Entrada)){} 
    while (!digitalRead(Entrada)){}
  }
  while (digitalRead(Entrada)){}
  while (!digitalRead(Entrada)){} //último período
  t = (micros() - t); //calcula o intervalo de tempo durante os períodos analisados
  
  tim = (float)t/(1000*Periodos); //calcula o período do sinal de entrada
  Serial.print("Periodo (ms): ");
  Serial.println(tim);
  freq = 1/(tim/1000); //calcula a frequência do sinal de entrada
  Serial.print("Frequencia (Hz): ");
  Serial.println(freq);
  
  T1=(tim*(ang)/360.0);      //tempo desligado do ciclo 
  T2=((T1-(int)T1)*1000);    //tempo desligado do ciclo (ajuste fino)
  T3=(tim*(180.0-ang)/360.0);//tempo ligado do ciclo 
  T4=((T3-(int)T3)*1000);    //tempo ligado do ciclo (ajuste fino)
  
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0*/
}

void loop() {
  while (digitalRead(Entrada)){} //trava o laço enquanto a entrada for 1
  delay(T1);             //tempo desligado do ciclo
  delayMicroseconds(T2); //tempo desligado do ciclo (ajuste fino)
  digitalWrite(Saida, HIGH);
  delay(T3);             //tempo ligado do ciclo
  delayMicroseconds(T4); //tempo ligado do ciclo (ajuste fino)
  digitalWrite(Saida, LOW);
  delay(tim*90/360);
}

/*
void loop(){
  digitalWrite(Saida, state);
}


void testar(){
  state != state;
}*/
