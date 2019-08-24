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

  #define  ANGULO_DE_ATIVACAO 90;  //Por padrão vem em 30, substitua o valor se quiser modificar

    //função para calcular a média de um vetor
    float returnFloatArrayAvg(float * argument, const int arraySize);

    //informações a serem enviadas pela conexão serial
    float freq; //frequência do sinal
    float tim; //tempo do período

    //delays usados para mandar os pulsos na hora certa
    float T1; //tempo desligado do ciclo
    float T2; //tempo desligado do ciclo (ajuste fino)
    
    //variáveis usadas para calcular a média móvel
    int Namostra=0;
    float inicioAmostra;
    float amostrasPeriodo[30];//amostrar usadas para calcular e atualizar o período

    //variáveis que eu posso transformar em macros acho
    float ang = ANGULO_DE_ATIVACAO; //angulo de ativação
    int Periodos = 30; //quantidade de períodos analisados para determinar a frequência


#define Entrada 3
#define Saida 13

void setup() {
  pinMode(Saida, OUTPUT);
  digitalWrite(Saida, LOW);
  pinMode(Entrada, INPUT);
  Serial.begin(9600);


  /*---------------------CÁLCULO INICIAL DO PERÍODO-----------------------*/
  while (digitalRead(Entrada)){}  //trava o laço enquanto a entrada for 1
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0
  inicioAmostra = micros();
  for (int p=0; p<Periodos; p++) //espera Períodos-1 ciclos  
  {
    while (digitalRead(Entrada)){}
    while (!digitalRead(Entrada)){}
    amostrasPeriodo[p]=(micros()-inicioAmostra);//00000
    inicioAmostra=micros();
  }
  tim = returnFloatArrayAvg(amostrasPeriodo, 30)/1000;//00000000000
  /*---------------------CÁLCULO INICIAL DO PERÍODO-----------------------*/

  
  Serial.print("Periodo (ms): ");
  Serial.println(tim);
  freq = 1/(tim/1000); //calcula a frequência do sinal de entrada
  Serial.print("Frequencia (Hz): ");
  Serial.println(freq);



  
  T1=(tim*(ang)/360.0);      //tempo desligado do ciclo 
  T2=((T1-(int)T1)*1000);    //tempo desligado do ciclo (ajuste fino)
  
  
  while (!digitalRead(Entrada)){} //trava o laço enquanto a entrada for 0
}

void loop() {

  /******************************///
  while (digitalRead(Entrada)){} //------->espera o inicio da parte positiva do ciclo
  /****************************///
  
  /************************///
  delay(T1);               //---------->tempo do ciclo em que o triac está aberto
  delayMicroseconds(T2);  //
  /*********************///
  
  /***************************///
  digitalWrite(Saida, HIGH);  //
  delayMicroseconds(100);    //------->pulso de 300uS
  digitalWrite(Saida, LOW); //
  /***********************///
  
  /*******************************///
  while (!digitalRead(Entrada)){} //------->espera o inicio da parte negativa do ciclo
  /*****************************///
   
  /*************************///
  delay(T1);                //---------->tempo do ciclo em que o triac está aberto
  delayMicroseconds(T2);   //
  /**********************///
  
  /***************************///
  digitalWrite(Saida, HIGH);  //
  delayMicroseconds(100);    //------->pulso de 300uS
  digitalWrite(Saida, LOW); //
  /***********************///

  /***********************************************************************************************************************///
  amostrasPeriodo[Namostra]=micros()-inicioAmostra; //guarda a duração do ultimo periodo no array                         //
  inicioAmostra=micros(); //atualiza a referência de inicio de período                                                   //
  Namostra++; //avança o endereço destino da amostra a ser gravada para o próximo elemento do array                     //
  Namostra = Namostra%Periodos; //volta o endereço para o primeiro elemento se o endereço novo estiver maior que 29    //
                                                                                                                      //-------->atualização da média móvel e dos valores de delay
  tim = returnFloatArrayAvg(amostrasPeriodo, 30)/1000; //calcula a nova média com o valor novo                       //
                                                                                                                    //
  T1=(tim*(ang)/360.0);      //atualiza o tempo desligado do ciclo                                                 //
  T2=((T1-(int)T1)*1000);    //atualiza o tempo desligado do ciclo (ajuste fino)                                  //
  /*************************************************************************************************************///

  /**********************************************************************///
  if(Namostra == 0){                                                     //
    Serial.println();                                                   //
    Serial.print("Periodo (ms): ");                                    //
    Serial.println(tim);                                              //------------------------>envio de dados para a porta serial a cada 30 períodos
    freq = 1/(tim/1000); //calcula a frequência do sinal de entrada  //
    Serial.print("Frequencia (Hz): ");                              //
    Serial.println(freq);                                          //
  /***************************************************************///
  }
  
}


//função para calcular a média de um vetor
float returnFloatArrayAvg(float * argument, const int arraySize){
  float arrayAvg=0;
  for(int i=0; i<arraySize;i++){
    arrayAvg += argument[i];
  }
  return (float)arrayAvg/arraySize;
}
