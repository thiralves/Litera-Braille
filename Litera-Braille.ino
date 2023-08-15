// **** Bibliotecas ****
#include <Servo.h>
#include <AFMotor.h>


// **** Definições ****
#define botao1 27 //porta serial
#define botao2 25 //porta serial
#define botao3 23 //porta serial
#define botao4 31 //porta serial
#define botao5 33 //porta serial
#define botao6 35 //porta serial
#define botaoEspaco 29 //porta serial
#define botaoLinha 37 //porta serial
#define botaoPosicaoInicial 39 //porta para botão que posiciona a página para inicar digitação
#define portaServo1_4 10
#define portaServo2_3 9
#define portaServo5_6 2
#define buzzer 51
#define fimDeCurso 48

/*
 * SINGLE: As bobinas são energizadas de maneira individual e possuem um torque razoável.
 * DOUBLE: As duas bobinas são alimntadas ao mesmo tempo, o que aumenta o torque
 * INTERLEAVE: Alterna entre as bobinas 1 e 2, aumentando a precisão
 * MICROSTEP: As bobinas adjacentes sofrem um movimento que cria um número de “micro pasos”, isso resulta em uma resolução mais fina e suave, porém faz com que o motor perca parte do seu torque.
 */
#define metodoMotorPasso INTERLEAVE

//Posições pré-definidas para os servomotores que marcam os pontos do Braille
const int centro = 90; // 90º
const int direita = 135; // 120º
const int esquerda = 45; // 60º - diferença de 30º entre o centro e o giro para um lado ou para o outro

//Definições necessárias para os motores de passo
const int passosDoMotor = 200; // quantidade de passos de uma revolução de 360º do motor Nema 17 cujos passos são de 1,8º
const int deslocamentoCaractere = 81;//40.5; // quantidade de passos do motor nema 17 para passar da posição de uma cela para a outra
const int deslocamentoLinha = 96;//46; // quantidade de passos do motor nema 17 para passar da posição de uma linha para a outra

// Velocidades
const int velocidadeEscrita = 80;
const int velocidadeMovimentacao = 120;

//Outras constantes
const int maxCaracteresLinha = 21; // ainda precisa calcular corretamente
const int maxLinhas = 30; // ainda precisa calcular corretamente
const int tempoEntreGiros = 100;
const int tempoRetornarCentro = 100;

// **** Variáveis ****
Servo servo1_4, servo5_6, servo2_3; //servo1_4, por exemplo, é responsável por marcar os pontos 1 e 4 da Cela Braille
AF_Stepper motorX(passosDoMotor, 2);
AF_Stepper motorY(passosDoMotor, 1);
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
int quantAletaracoesLado = 3;
int alteracaoLado = 20;

// **** Declaração das funções ****
void lerBotoes();
bool temBotaoPressionado();
void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6, int status_botaoEspaco);
void posicionarProximaLinha();
void posicionarProximoCaractere();
void voltarInicioLinha();
void posicionaInicioLinha();
void liberarPagina();
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
  pinMode(botaoPosicaoInicial, INPUT);
  
  servo1_4.attach(portaServo1_4);
  servo2_3.attach(portaServo2_3);
  servo5_6.attach(portaServo5_6);
  
  servo1_4.write(centro);
  servo2_3.write(centro);
  servo5_6.write(centro);

  motorX.setSpeed(velocidadeEscrita);  
  motorY.setSpeed(velocidadeEscrita);

   pinMode(buzzer, OUTPUT);
   pinMode(fimDeCurso, INPUT);

   posicionaInicioLinha();
}

void loop() {
  lerBotoes();
  /*if (digitalRead(botaoReset)){
	  asm volatile ("  jmp 0");
  }*/

  if (digitalRead(botaoPosicaoInicial)){
    posicionaInicioLinha();
    puxarNovaPagina();
  }
  
  if (status_botaoLinha){
    if(!digitalRead(botaoLinha)){
      posicionaProximaLinha();
      resetarStatus();
      countCaracteres = 0;
    }
  }else{
    if((status_botao1 && !digitalRead(botao1)) || (status_botao2 && !digitalRead(botao2)) || (status_botao3 && !digitalRead(botao3)) || (status_botao4 && !digitalRead(botao4))
               || (status_botao5 && !digitalRead(botao5)) || (status_botao6 && !digitalRead(botao6)) || (status_botaoEspaco && !digitalRead(botaoEspaco))){
      while(temBotaoPressionado()){
        lerBotoes();
		    delay(10);
	    }
	  marcarPontos(status_botao1, status_botao2, status_botao3, status_botao4, status_botao5, status_botao6);
    //posicionarProximoCaractere();
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

void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6){
  if(status_botao1){
    servo1_4.write(esquerda);
    delay(50);
  }
  if(status_botao3){
    servo2_3.write(esquerda);
    delay(50);
  }
  if(status_botao5){
    servo5_6.write(esquerda);
    delay(50);
  }

  if(status_botao1 || status_botao3 || status_botao5){
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
  }
  
  if(status_botao2){
    servo2_3.write(direita);
    delay(50);
  }
  if(status_botao4){
    servo1_4.write(direita);
    delay(50);
  }
  if(status_botao6){
    servo5_6.write(direita);
    delay(50);
    //girarServoMotor("direito", servo5_6);
  }

  if(status_botao2 || status_botao4 || status_botao6){
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
  }
  posicionarProximoCaractere();
}

void girarServoMotor(String lado, Servo &servo){
  int valor = centro;
  Serial.println("lado = "+lado+", alteração = "+alteracaoLado);
  if (lado == "esquerdo"){
    Serial.println("entrou no if");
    alteracaoLado = -alteracaoLado;
  }
  for (int i = 0; i < quantAletaracoesLado; i++){
    valor = valor + alteracaoLado;
    servo.write(valor);
    delay(tempoEntreGiros);
  }
}


void posicionarProximoCaractere(){
  countCaracteres++;
  if (countCaracteres >= maxCaracteresLinha - 2){
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }
  if (countCaracteres >= maxCaracteresLinha - 1){
    delay(100);
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }
  if (countCaracteres >= maxCaracteresLinha){
    //posicionarProximaLinha(countCaracteres);
    posicionaProximaLinha();
    countCaracteres = 0;
    return;
  }
  motorX.step(deslocamentoCaractere, BACKWARD, metodoMotorPasso);
  motorX.release();
  //Serial.println("Entrou para alimentar o motor X");
}

void posicionaProximaLinha(){
  posicionaInicioLinha();
  countLinhas++;
  if (countLinhas >= maxLinhas){
    liberarPagina();
    countLinhas = 0;
    return;
  }
  motorY.step(deslocamentoLinha, BACKWARD, metodoMotorPasso);
  motorY.release();
}

void posicionarProximaLinha(int quantCarecteresDeslocamento){
  voltarInicioLinha(quantCarecteresDeslocamento);
  countLinhas++;
  if (countLinhas >= maxLinhas){
    liberarPagina();
    countLinhas = 0;
    return;
  }
  motorY.step(deslocamentoLinha, BACKWARD, metodoMotorPasso);
  motorY.release();
}

void posicionaInicioLinha(){
  motorX.setSpeed(velocidadeMovimentacao);
  while(!digitalRead(fimDeCurso)){
    motorX.step(1, FORWARD, metodoMotorPasso);
  }
  motorX.release();
  motorX.setSpeed(velocidadeEscrita);
}

void voltarInicioLinha(int quantCarecteresDeslocamento){
  int quantPassosRetornar = quantCarecteresDeslocamento * deslocamentoCaractere;
  motorX.setSpeed(velocidadeMovimentacao);
  motorX.step(quantPassosRetornar, FORWARD, metodoMotorPasso);
  motorX.release();
  motorX.setSpeed(velocidadeEscrita);
}

 void liberarPagina(){
  int deslocamentoLiberarPagina = deslocamentoLinha * 3; //quantidade de linhas para o final da página
  motorY.setSpeed(velocidadeMovimentacao);
  motorY.step(deslocamentoLiberarPagina, BACKWARD, metodoMotorPasso);
  motorY.release();
  motorY.setSpeed(velocidadeEscrita);
 }

void puxarNovaPagina(){ //acionado pelo botão de posicionar nova página - Futuramente pode ser controlado por um sensor que identifique o término do papel e início de outro
  int deslocamentoPosicionarNovaPagina = deslocamentoLinha * 6; //quantidade de linhas para o final da página
  motorY.setSpeed(velocidadeMovimentacao);
  motorY.step(deslocamentoPosicionarNovaPagina, BACKWARD, metodoMotorPasso);
  motorY.release();
  motorY.setSpeed(velocidadeEscrita);
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
    if (alteracaoLado<0){
      alteracaoLado = -alteracaoLado;
    }
}
