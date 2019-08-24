/***********************************************************************
 *   ________ _______  ________    __________________              _
 *  |   ___  | __    ||   _____|  /           | ____  \              \
 *  |  |___| || _|  / |  |___    /            | |   | |     0         |
 *  |  ______|  _   \ |   ___|  |      +      | |   | |        -----  |
 *  |  |   |   | \   \|  |_____ |            /  |__/ /      |         |
 *  |__|   |___|  \___\________||___________/|______/              _ /
 *  
 *  
 *  IFSC - Câmpus Florianópolis
 *  Eletrônica de Potência, Professor Petry 2019-2
 *  Programa para o laboratório #24 - Controle de ângulo de fase
 *  
 *  Feito por Pedro de Lima Batista,
 *  modificado a partir de...
  ***********************************************************************/

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
#define  ANGULO_DE_ATIVACAO 90  //Por padrão vem em 90, substitua o valor se quiser modificar


  

/*-------------------PROTÓTIPOS DE FUNÇÕES------------------------------------------*/

//função para calcular a média de um vetor
float returnFloatArrayAvg(float * argument, const int arraySize);
    
//função para mandar o pulso na saída
void pulso(const int pino);

//função para delays com milissegundos de duração com precisão de microssegundos
void esperaPrecisa(float espera);

/*-------------------PROTÓTIPOS DE FUNÇÕES------------------------------------------*/




//informações a serem enviadas pela conexão serial
float freq; //frequência do sinal
float tim; //tempo do período



/*----MACROS----*/

#define Entrada 3
#define EntrDescida 2
#define Saida 13
#define Periodos 30
/*----MACROS----*/





/*--VARIÁVEIS PARA GUARDAR OS VALORES DOS DELAYS USADOS PARA MANDAR OS PULSOS NA HORA CERTA--*/

 float meio;//------>tempo entre o inicio do período e 90º 
 float distPulsos;//---------------------------------------->distância entre pulsos (sempre com valor igual a 180º do ciclo
 float correcao;//---->desvio do pulso em relação ao meio 

 float metDurNeg;//----> metade da duração negativa do
  
/*--VARIÁVEIS PARA GUARDAR OS VALORES DOS DELAYS USADOS PARA MANDAR OS PULSOS NA HORA CERTA--*/





/*-------------------------VARIÁVEIS USADAS PARA CALCULAR A MÉDIA MÓVEL------------------------*/

 int Namostra=0;
 float inicioAmostra;
 float inicioAmostraDurNeg;
 float amostrasPeriodo[30];//amostrar usadas para calcular e atualizar o período
 float amostrasDurNeg[30];
 
/*-------------------------VARIÁVEIS USADAS PARA CALCULAR A MÉDIA MÓVEL------------------------*/



void setup() {
  attachInterrupt(digitalPinToInterrupt(Entrada),inicNeg,RISING); 
  attachInterrupt(digitalPinToInterrupt(EntrDescida),finNeg,FALLING);

  pinMode(Saida, OUTPUT);
  digitalWrite(Saida, LOW);
  pinMode(Entrada, INPUT);
  pinMode(EntrDescida, INPUT);
  Serial.begin(9600);


  /*---------------------CÁLCULO INICIAL DO PERÍODO-----------------------*/
  
  while (digitalRead(Entrada)){}  //trava o laço enquanto a entrada for 1
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0
  inicioAmostra = micros();
  for (int p=0; p<Periodos; p++) //espera Períodos-1 ciclos  
  {
    while (digitalRead(Entrada)){}
    amostrasDurNeg[p]=(micros()-inicioAmostra);
    while (!digitalRead(Entrada)){}
    amostrasPeriodo[p]=(micros()-inicioAmostra);
    inicioAmostra=micros();
  }
  tim = returnFloatArrayAvg(amostrasPeriodo, 30)/1000;
  
  /*---------------------CÁLCULO INICIAL DO PERÍODO-----------------------*/

  
  Serial.print("Periodo (ms): ");
  Serial.println(tim);
  freq = 1/(tim/1000); //calcula a frequência do sinal de entrada
  Serial.print("Frequencia (Hz): ");
  Serial.println(freq);


  meio=tim*0.25;//----------------------------------->tempo entre o inicio do período e 90º  
  distPulsos=tim*0.5;//-------------------------------------------------------------------->distância entre o pulso da metade positiva do período e o da negativa (sempre 180º)
  correcao = (tim*(ANGULO_DE_ATIVACAO-90.0)/360.0);//---->desvio do pulso em relação ao meio

  interrupts();
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0
}

void loop() {

  while (((micros()-inicioAmostraDurNeg)/1000)<(metDurNeg+correcao)){}

  pulso(Saida);
  
  esperaPrecisa(distPulsos);     //180º
    
  pulso(Saida);
  
  while(!digitalRead(Entrada)); 

  
  
  /***********************************************************************************************************************///
  amostrasPeriodo[Namostra]=micros()-inicioAmostra; //guarda a duração do ultimo periodo no array                         //
  inicioAmostra=micros(); //atualiza a referência de inicio de período                                                   //
  Namostra++; //avança o endereço destino da amostra a ser gravada para o próximo elemento do array                     //
  Namostra = Namostra%Periodos; //volta o endereço para o primeiro elemento se o endereço novo estiver maior que 29    //
                                                                                                                      //-------->atualização da média móvel e dos valores de delay
  tim = returnFloatArrayAvg(amostrasPeriodo, 30)/1000; //calcula a nova média com o valor novo                       //
  metDurNeg = returnFloatArrayAvg(amostrasDurNeg, 30)/2000;                                                         //
                                                                                                                   //
  correcao = (tim*(ANGULO_DE_ATIVACAO-90.0)/360.0);      //atualiza o valor                                       //
  /*************************************************************************************************************///

  /***********************************************************************///
  if(Namostra == 0){                                                      //
    /*Serial.println();                                                    //
    Serial.print("Periodo (ms): ");                                     //
    Serial.println(tim);                                               //------------------------>envio de dados para a porta serial a cada 30 períodos
    freq = 1/(tim/1000); //calcula a frequência do sinal de entrada   //
    Serial.print("Frequencia (Hz): ");   */                            //
    //Serial.println(freq);                                           //
    //Serial.println(correcao);
    //Serial.println(metDurNeg);
   }                                                               //
  /**************************************************************///
  
  
}




//função para calcular a média de um vetor
float returnFloatArrayAvg(float * argument, const int arraySize){
  float arrayAvg=0;
  for(int i=0; i<arraySize;i++){
    arrayAvg += argument[i];
  }
  return (float)arrayAvg/arraySize;
}

//função para enviar pulso
void pulso(const int pino){
  digitalWrite(pino, HIGH);  //
  delayMicroseconds(300);    //------->pulso de 300uS
  digitalWrite(pino, LOW);
}

//função para delays com milissegundos de duração com precisão de microssegundos
void esperaPrecisa(float espera){
  delay(espera);
  delayMicroseconds(((espera-(int)espera)*1000));
}

void inicNeg(){
  inicioAmostraDurNeg = micros();
}

void finNeg(){
  amostrasDurNeg[Namostra]=micros()-inicioAmostraDurNeg;
}
