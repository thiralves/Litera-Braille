// **** Bibliotecas ****
#include <Servo.h>



// **** Constantes ****
#define botao1 22 //porta serial
#define botao2 24 //porta serial
#define botao3 26 //porta serial
#define botao4 28 //porta serial
#define botao5 30 //porta serial
#define botao6 32 //porta serial
#define botaoEspaco 34 //porta serial
#define botaoLinha 36 //porta serial
#define botaoReset 40 // botão para reiniciar o programa, por exemplo, para iniciar uma digitação

#define portaServo1_4 2 
#define portaServo2_3 3
#define portaServo5_6 4

#define motorLetrasDir 8
#define motorLetrasEsq 9

#define motorLinhaFrente 10
#define motorLinhaTras 11

#define espaco 100 // PWM de 0 a 255
#define linha 200 // PWM de 0 a 255

//Posições pré-definidas para os servomotores que marcam os pontos do Braille
#define centro 90 // 90º
#define direita 70 // 70º
#define esquerda 110 // 110º - diferença de 20º entre o centro e o giro para um lado ou para o outro

// **** Variáveis ****
Servo servo1_4, servo2_3, servo5_6; //servo1_4, por exemplo, é responsável por marcar os pontos 1 e 4 da Cela Braille
int status_botao1 = 0;
int status_botao2 = 0;
int status_botao3 = 0;
int status_botao4 = 0;
int status_botao5 = 0;
int status_botao6 = 0;
int status_botaoEspaco = 0;
int status_botaoLinha = 0;
int countCaracteres = 0;
int countLinhas = 0;
int maxCaracteresLinha = 30; // ainda precisa calcular corretamente
int maxLinhas = 30; // ainda precisa calcular corretamente
int tempoEntreGiros = 300;
int tempoSaltarLinha = 50; // ainda precisa calcular corretamente
int tempoSaltarEspaco = 50; // ainda precisa calcular corretamente
int tempoPercorrerLinha = 300; // ainda precisa calcular corretamente
int tempoAjustarNovaPagina = 500; // ainda precisa calcular corretamente

// **** Declaração das funções ****
void lerBotoes();
bool temBotaoPressionado();
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
  if (digitalRead(botaoReset)){
	  asm volatile ("  jmp 0");
  }
  if (status_botaoLinha){
    if(!digitalRead(botaoLinha)){
      saltarLinha();
      resetarStatus();
    }
  }else{
    if((status_botao1 && !digitalRead(botao1)) || (status_botao2 && !digitalRead(botao2)) || (status_botao3 && !digitalRead(botao3)) || (status_botao4 && !digitalRead(botao4))
               || (status_botao5 && !digitalRead(botao5)) || (status_botao6 && !digitalRead(botao6)) || (status_botaoEspaco && !digitalRead(botaoEspaco))){
	  //delay(300);
	  while(temBotaoPressionado()){
		delay(10);
	  }
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

bool temBotaoPressionado(){
	return digitalRead(botao1) || digitalRead(botao2) || digitalRead(botao3) || digitalRead(botao4) || digitalRead(botao5) || digitalRead(botao6) || digitalRead(botaoEspaco);
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
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
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
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
  }

  saltarEspaco();
}

void saltarEspaco(){
  countCaracteres++;
  if (countCaracteres >= maxCaracteresLinha){
    saltarLinha();
    countCaracteres = 0;
    return;
  }
  analogWrite(motorLetrasDir, espaco);
  delay(tempoSaltarEspaco);
  digitalWrite(motorLetrasDir, LOW);
}

void saltarLinha(){
  voltarInicioLinha();
  countLinhas++;
  if (countLinhas >= maxLinhas){
    puxarNovaPagina();
    countLinhas = 0;
    return;
  }
  analogWrite(motorLinhaFrente, linha);
  delay(tempoSaltarLinha);
  digitalWrite(motorLinhaFrente, LOW);
}

void voltarInicioLinha(){
  analogWrite(motorLetrasEsq, espaco);
  delay(tempoPercorrerLinha);
  digitalWrite(motorLetrasEsq, LOW);
}

void puxarNovaPagina(){
  analogWrite(motorLinhaFrente, linha);
  delay(tempoAjustarNovaPagina);
  digitalWrite(motorLinhaFrente, LOW);
}

void resetarStatus(){
    status_botao1 = 0;
    status_botao2 = 0;
    status_botao3 = 0;
    status_botao4 = 0;
    status_botao5 = 0;
    status_botao6 = 0;
    status_botaoEspaco = 0;
    status_botaoLinha = 0;
}