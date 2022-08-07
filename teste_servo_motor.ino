// **** Bibliotecas ****
#include <Servo.h>


// **** Constantes ****
#define botao1 22 //porta serial
#define botao2 23 //porta serial
#define botao3 24 //porta serial
#define botao4 25 //porta serial
#define botao5 26 //porta serial
#define botao6 27 //porta serial
#define botaoEspaco 28 //porta serial
#define botaoLinha 29 //porta serial

#define portaServo1_4 2 
#define portaServo2_3 3
#define portaServo5_6 4

#define motorLetrasDir 8
#define motorLetrasEsq 9

#define motorLinhaFrente 10
#define motorLinhaTras 11

#define espaco 100 // porta analógica PWM de 0 a 255
#define linha 200

//Posições pré-definidas para os servomotores que marcam os pontos do Braille
#define centro 90
#define direita 60
#define esquerda 120


// **** Variáveis ****
Servo servo1_4, servo2_3, servo5_6;
int status_botao1 = 0; // 1 <- Identificador
int status_botao2 = 0; // 2 <- Identificador
int status_botao3 = 0; // 3 <- Identificador
int status_botao4 = 0; // 4 <- Identificador
int status_botao5 = 0; // 5 <- Identificador
int status_botao6 = 0; // 6 <- Identificador
int status_botaoEspaco = 0; // 7 <- Identificador
int status_botaoLinha = 0; // 8 <- Identificador
int countCaracteres = 0;
int countLinhas = 0;
int maxCaracteresLinha = 30;
int maxLinhas = 30;


// **** Declaração das funções ****
void lerBotoes();
void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6, int status_botaoEspaco);
void saltarLinha();
void saltarEspaco();
void voltarInicioLinha();
void puxarNovaPagina();
void resetarStatus();

void setup() {

  Serial.begin(9600);

  //Define os pinos dos botões como entrada
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(botao3, INPUT);
  pinMode(botao4, INPUT);
  pinMode(botao5, INPUT);
  pinMode(botao6, INPUT);
  pinMode(botaoEspaco, INPUT);
  pinMode(botaoLinha, INPUT);

  //Define os pinos dos motores como saida
  pinMode(motorLetrasDir, OUTPUT);
  pinMode(motorLetrasEsq, OUTPUT);
  pinMode(motorLinhaFrente, OUTPUT);
  pinMode(motorLinhaTras, OUTPUT);
  
  servo1_4.attach(portaServo1_4);
  servo2_3.attach(portaServo2_3);
  servo5_6.attach(portaServo5_6);
  
  servo1_4.write(centro);
  servo2_3.write(centro);
  servo5_6.write(centro);
  
}

void loop() {
  lerBotoes();
  if (status_botaoLinha){
    if(!digitalRead(botaoLinha)){
      saltarLinha();
      resetarStatus();
    }
  }else{
    if((status_botao1 && !digitalRead(botao1)) || (status_botao2 && !digitalRead(botao2)) || (status_botao3 && !digitalRead(botao3)) || (status_botao4 && !digitalRead(botao4))
               || (status_botao5 && !digitalRead(botao5)) || (status_botao6 && !digitalRead(botao6)) || (status_botaoEspaco && !digitalRead(botaoEspaco))){
      marcarPontos(status_botao1, status_botao2, status_botao3, status_botao4, status_botao5, status_botao6, status_botaoEspaco);
      resetarStatus();
    }
  }
}



void lerBotoes(){

  if(digitalRead(botao1)){
    status_botao1 = 1;
  }
  
  if(digitalRead(botao2)){
    status_botao2 = 1;
  }
  
  if(digitalRead(botao3)){
    status_botao3 = 1;
  }
  
  if(digitalRead(botao4)){
    status_botao4 = 1;
  }
  
  if(digitalRead(botao5)){
    status_botao5 = 1;
  }
  
  if(digitalRead(botao6)){
    status_botao6 = 1;
  }

  if(digitalRead(botaoEspaco)){
    status_botaoEspaco = 1;
  }

  if(digitalRead(botaoLinha)){
    status_botaoLinha = 1;
  }
}

void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6, int status_botaoEspaco){
  if(status_botao1){
    servo1_4.write(direita);
  }
  if(status_botao3){
    servo2_3.write(direita);
  }
  if(status_botao5){
    servo5_6.write(direita);
  }

  if(status_botao1 || status_botao3 || status_botao5){
    delay(200);
  }
  
  if(status_botao2){
    servo2_3.write(esquerda);
  }
  if(status_botao4){
    servo1_4.write(esquerda);
  }
  if(status_botao6){
    servo5_6.write(esquerda);
  }

  if(status_botao2 || status_botao4 || status_botao6){
    delay(200);
  }
  
  servo1_4.write(centro);
  servo2_3.write(centro);
  servo5_6.write(centro);

  saltarEspaco();
  
}

void saltarLinha(){
  analogWrite(motorLinhaFrente, linha);
  delay(50);
  digitalWrite(motorLinhaFrente, LOW);
  countLinhas++;
  if (countLinhas >= maxLinhas){
    puxarNovaPagina();
    countLinhas = 0;
  }
}

void saltarEspaco(){
  analogWrite(motorLetrasDir, espaco);
  delay(50);
  digitalWrite(motorLetrasDir, LOW);
  countCaracteres++;
  if (countCaracteres >= maxCaracteresLinha){
    saltarLinha();
    voltarInicioLinha();
    countCaracteres = 0;
  }
}

void voltarInicioLinha(){
  
}

void puxarNovaPagina(){
  
}

void resetarStatus(){
    status_botao1 = 0; // 1 <- Identificador
    status_botao2 = 0; // 2 <- Identificador
    status_botao3 = 0; // 3 <- Identificador
    status_botao4 = 0; // 4 <- Identificador
    status_botao5 = 0; // 5 <- Identificador
    status_botao6 = 0; // 6 <- Identificador
    status_botaoEspaco = 0; // 7 <- Identificador
    status_botaoLinha = 0; // 8 <- Identificador
}
