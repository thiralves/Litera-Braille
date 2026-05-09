// **** Bibliotecas ****
#include <Servo.h>
#include <AFMotor.h>

#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini player;



// **** Definições ****
#define botao1 27 //porta serial
#define botao2 25 //porta serial
#define botao3 23 //porta serial
#define botao4 31 //porta serial
#define botao5 33 //porta serial
#define botao6 35 //porta serial
#define botaoEspaco 29 //porta serial
#define botaoApagar 37 //porta serial
#define botaoRetroceder 39 //porta para botão que retorna a cabeça de impressão para aposição do caractere anterior
#define botaoPosicaoInicial 41 //porta para botão que posiciona a página para inicar digitação
#define botaoLinha 45 //porta serial
#define portaServo1_4 2
#define portaServo2_3 10
#define portaServo5_6 9
#define buzzer 51
#define fimDeCurso 48
#define solenoide 47

/*
 * SINGLE: As bobinas são energizadas de maneira individual e possuem um torque razoável.
 * DOUBLE: As duas bobinas são alimntadas ao mesmo tempo, o que aumenta o torque
 * INTERLEAVE: Alterna entre as bobinas 1 e 2, aumentando a precisão
 * MICROSTEP: As bobinas adjacentes sofrem um movimento que cria um número de “micro pasos”, isso resulta em uma resolução mais fina e suave, porém faz com que o motor perca parte do seu torque.
 */
#define metodoMotorPasso INTERLEAVE

//Posições pré-definidas para os servomotores que marcam os pontos do Braille
//const int centro = 90; // 90º
const int centro_servo1_4 = 92;
const int centro_servo2_3 = 84;
const int centro_servo5_6 = 80;
int direita = 135; // 120º
int esquerda = 45; //60º

//Definições necessárias para os motores de passo
const int passosDoMotor = 200; // quantidade de passos de uma revolução de 360º do motor Nema 17 cujos passos são de 1,8º
const int deslocamentoCaractere = 80.95;//40.5; // quantidade de passos do motor nema 17 para passar da posição de uma cela para a outra
const int deslocamentoLinha = 150;//96;//48; // quantidade de passos do motor nema 17 para passar da posição de uma linha para a outra
const int deslocamentoPapelApagarCaractere = 4.7 * deslocamentoLinha;

// Velocidades
const int velocidadeEscrita = 80;
const int velocidadeMovimentacao = 120;

//Outras constantes
const int maxCaracteresLinha = 22;
const int maxLinhas = 10; // ainda precisa calcular corretamente
const int tempoSensibilidadeBotoes = 100; // tempo para reduzir sensibilidade nos botões. Ou seja, só começa a valer o botão pressionado após esse tempo
//const int tempoEntreGiros = 100;
//const int tempoRetornarCentro = 100;

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
int countCaracteres = 0;
int countLinhas = 0;
//int quantAletaracoesLado = 3;
//int alteracaoLado = 20;

// **** Declaração das funções ****
void lerBotoes();
bool temBotaoPressionado();
bool temBotaoComStatusAtivo();
bool temBotaoNaoPressionadoComStatusAtivo();
void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6, int status_botaoEspaco);
void posicionarProximaLinha();
void posicionarProximoCaractere();
void retornarPosicaoCarectereAnterior();
void voltarInicioLinha();
void posicionaInicioLinha();
void liberarPagina();
void puxarNovaPagina();
void resetarStatus();
void apagarCaractere();

String textoLinha = "";
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600); // HC05

  // ---------------- BOTÕES ----------------
  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(botao3, INPUT);
  pinMode(botao4, INPUT);
  pinMode(botao5, INPUT);
  pinMode(botao6, INPUT);
  pinMode(botaoEspaco, INPUT);
  pinMode(botaoRetroceder, INPUT);
  pinMode(botaoApagar, INPUT);
  pinMode(botaoLinha, INPUT);
  pinMode(botaoPosicaoInicial, INPUT);

  // ---------------- SERVOS ----------------
  servo1_4.attach(portaServo1_4);
  servo2_3.attach(portaServo2_3);
  servo5_6.attach(portaServo5_6);

  servo1_4.write(centro_servo1_4);
  servo2_3.write(centro_servo2_3);
  servo5_6.write(centro_servo5_6);

  // ---------------- MOTORES ----------------
  motorX.setSpeed(velocidadeEscrita);  
  motorY.setSpeed(velocidadeEscrita);

  // ---------------- OUTROS ----------------
  pinMode(buzzer, OUTPUT);
  pinMode(fimDeCurso, INPUT);
  pinMode(solenoide, OUTPUT);
  digitalWrite(solenoide, LOW);

  posicionaInicioLinha();

  // ---------------- DFPLAYER ----------------
  Serial1.begin(9600);
  delay(1000); // 

  Serial.println("Iniciando DFPlayer...");

  if (!player.begin(Serial1)) {
    Serial.println("❌ DFPlayer NAO CONECTOU");
  } else {
    Serial.println("✅ DFPlayer OK");

    player.volume(30);   // volume (0–30)
    delay(500);

    // 🔊 SOM DE INICIALIZAÇÃO (opcional)
    player.playFolder(2, 1);
  }
}



void tocarLetra(char letra) {
  if (!player.available()) return;
  
  int numero = letra - 'A' + 1; // A=1, B=2...
  Serial.println(numero);
  Serial.print("Tocando letra: ");
  Serial.println(letra);
  
  player.playFolder(1, numero);
  delay(300); // evita bug do DFPlayer
}


char identificarLetra() {

  // A (1)
  if (status_botao3 && !status_botao2 && !status_botao1 && !status_botao6 && !status_botao5 && !status_botao4)
    return 'A';

  // B (1 2)
  if (status_botao3 && status_botao2 && !status_botao1 && !status_botao6 && !status_botao5 && !status_botao4)
    return 'B';

  // C (1 4)
  if (status_botao3 && status_botao6 && !status_botao2 && !status_botao1 && !status_botao5 && !status_botao4)
    return 'C';

  // D (1 4 5)
  if (status_botao3 && status_botao6 && status_botao5 && !status_botao2 && !status_botao1 && !status_botao4)
    return 'D';

  // E (1 5)
  if (status_botao3 && status_botao5 && !status_botao2 && !status_botao1 && !status_botao6 && !status_botao4)
    return 'E';

  // F (1 2 4)
  if (status_botao3 && status_botao2 && status_botao6 && !status_botao1 && !status_botao5 && !status_botao4)
    return 'F';

  // G (1 2 4 5)
  if (status_botao3 && status_botao2 && status_botao6 && status_botao5 && !status_botao1 && !status_botao4)
    return 'G';

  // H (1 2 5)
  if (status_botao3 && status_botao2 && status_botao5 && !status_botao1 && !status_botao6 && !status_botao4)
    return 'H';

  // I (2 4)
  if (!status_botao3 && status_botao2 && status_botao6 && !status_botao1 && !status_botao5 && !status_botao4)
    return 'I';

  // J (2 4 5)
  if (!status_botao3 && status_botao2 && status_botao6 && status_botao5 && !status_botao1 && !status_botao4)
    return 'J';

  // K (1 3)
  if (status_botao3 && status_botao1 && !status_botao2 && !status_botao6 && !status_botao5 && !status_botao4)
    return 'K';

  // L (1 2 3)
  if (status_botao3 && status_botao2 && status_botao1 && !status_botao6 && !status_botao5 && !status_botao4)
    return 'L';

  // M (1 3 4)
  if (status_botao3 && status_botao1 && status_botao6 && !status_botao2 && !status_botao5 && !status_botao4)
    return 'M';

  // N (1 3 4 5)
  if (status_botao3 && status_botao1 && status_botao6 && status_botao5 && !status_botao2 && !status_botao4)
    return 'N';

  // O (1 3 5)
  if (status_botao3 && status_botao1 && status_botao5 && !status_botao2 && !status_botao6 && !status_botao4)
    return 'O';

  // P (1 2 3 4)
  if (status_botao3 && status_botao2 && status_botao1 && status_botao6 && !status_botao5 && !status_botao4)
    return 'P';

  // Q (1 2 3 4 5)
  if (status_botao3 && status_botao2 && status_botao1 && status_botao6 && status_botao5 && !status_botao4)
    return 'Q';

  // R (1 2 3 5)
  if (status_botao3 && status_botao2 && status_botao1 && status_botao5 && !status_botao6 && !status_botao4)
    return 'R';

  // S (2 3 4)
  if (!status_botao3 && status_botao2 && status_botao1 && status_botao6 && !status_botao5 && !status_botao4)
    return 'S';

  // T (2 3 4 5)
  if (!status_botao3 && status_botao2 && status_botao1 && status_botao6 && status_botao5 && !status_botao4)
    return 'T';

  // U (1 3 6)
  if (status_botao3 && status_botao1 && status_botao4 && !status_botao2 && !status_botao6 && !status_botao5)
    return 'U';

  // V (1 2 3 6)
  if (status_botao3 && status_botao2 && status_botao1 && status_botao4 && !status_botao6 && !status_botao5)
    return 'V';

  // W (2 4 5 6)
  if (!status_botao3 && status_botao2 && !status_botao1 && status_botao6 && status_botao5 && status_botao4)
    return 'W';

  // X (1 3 4 6)
  if (status_botao3 && status_botao1 && status_botao6 && status_botao4 && !status_botao2 && !status_botao5)
    return 'X';

  // Y (1 3 4 5 6)
  if (status_botao3 && status_botao1 && status_botao6 && status_botao5 && status_botao4 && !status_botao2)
    return 'Y';

  // Z (1 3 5 6)
  if (status_botao3 && status_botao1 && status_botao5 && status_botao4 && !status_botao2 && !status_botao6)
    return 'Z';

  return '?';
}


void tocarComando(int numero) {
  if (!player.available()) return;

  Serial.print("Tocando comando: ");
  Serial.println(numero);

  player.playFolder(3, numero); // pasta 3 (comandos)
  delay(300);
}




void loop() {

  if (digitalRead(botaoPosicaoInicial)){
    delay(tempoSensibilidadeBotoes);
    if (digitalRead(botaoPosicaoInicial)){
      tocarComando(5); // 001 = Enter
      Serial2.println(textoLinha);
      Serial.print("Enviado Bluetooth: ");
      Serial.println(textoLinha);

  textoLinha = "";

      posicionaInicioLinha();
      puxarNovaPagina();
    }
  }

  if (digitalRead(botaoLinha)){
    delay(tempoSensibilidadeBotoes);
    if (digitalRead(botaoLinha)){
      tocarComando(2); // nova linha
      posicionaProximaLinha();
      countCaracteres = 0;
    }
  }

  if (digitalRead(botaoRetroceder)){
    delay(tempoSensibilidadeBotoes);
    if (digitalRead(botaoRetroceder)){
      tocarComando(4); // retroceder
      retornarPosicaoCarectereAnterior();
      if (textoLinha.length() > 0) {  //Apagando do buffer
        textoLinha.remove(textoLinha.length() - 1);
}
    }
  }

  if (digitalRead(botaoApagar)){
    delay(tempoSensibilidadeBotoes);
    if (digitalRead(botaoApagar)){
      tocarComando(1); //apagar
      apagarCaractere();
    }
  }

  if (digitalRead(botaoEspaco)) {
  delay(tempoSensibilidadeBotoes);
  if (digitalRead(botaoEspaco)) {
    tocarComando(3); // espaço
    textoLinha += " "; //adicionado no buffer
    posicionarProximoCaractere();
    return; // importante pra não cair na leitura de letra
  }
}
  
  //lerBotoes();

  /*if (digitalRead(botaoReset)){
    asm volatile ("  jmp 0");
  }*/  
  
  if (temBotaoPressionado()){
    delay(tempoSensibilidadeBotoes);
    lerBotoes();

    if (temBotaoComStatusAtivo()){
      if(temBotaoNaoPressionadoComStatusAtivo()){
          while(temBotaoPressionado()){
            lerBotoes(); 
            delay(10);
          }
          marcarPontos(status_botao1, status_botao2, status_botao3, status_botao4, status_botao5, status_botao6);
          //posicionarProximoCaractere();
          char letra = identificarLetra();

          if (letra != '?') {

            tocarLetra(letra);

            textoLinha += letra;

            Serial.print("Texto atual: ");
            Serial.println(textoLinha);
          }
          resetarStatus();
      }
      
    }
  }
  
}

void lerBotoes(){

  Serial.println("Método lerBotoes");

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
  
}

bool temBotaoPressionado(){
  return digitalRead(botao1) || digitalRead(botao2) || digitalRead(botao3) || digitalRead(botao4) || digitalRead(botao5) || digitalRead(botao6) || digitalRead(botaoEspaco);
}

bool temBotaoComStatusAtivo(){
  Serial.println("Método temBotaoComStatusAtivo");
  return status_botao1 || status_botao2 || status_botao3 || status_botao4 || status_botao5 || status_botao6 || status_botaoEspaco;
}

bool temBotaoNaoPressionadoComStatusAtivo(){
  Serial.println("Método temBotaoNaoPressionadoComStatusAtivo");
  return (status_botao1 && !digitalRead(botao1)) || (status_botao2 && !digitalRead(botao2)) || (status_botao3 && !digitalRead(botao3)) || (status_botao4 && !digitalRead(botao4))
                 || (status_botao5 && !digitalRead(botao5)) || (status_botao6 && !digitalRead(botao6)) || (status_botaoEspaco && !digitalRead(botaoEspaco));
}

/*void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6){
  Serial.println("Método marcarPontos");
  int tempoEntreGiros = 200;
  int tempoRetornarCentro = 200;
  int tempoEntreServos = 100;
  int fator = 10;
  direita = 120;
  esquerda = 60;
  if(status_botao1){
    esquerda -= fator;
    if(status_botao3){
      esquerda -= fator;
      if(status_botao5){
        esquerda -= fator;
        servo5_6.write(esquerda);
        delay(tempoEntreServos);
      }
      servo2_3.write(esquerda);
      delay(tempoEntreServos);
    }else{
      if(status_botao5){
        esquerda -= fator;
        servo5_6.write(esquerda);
        delay(tempoEntreServos);
      }
    }
    servo1_4.write(esquerda);
    delay(tempoEntreServos);
  }else{
    if(status_botao3){
      esquerda -= fator;
      if(status_botao5){
        esquerda -= fator;
        servo5_6.write(esquerda);
        delay(tempoEntreServos);
      }
      servo2_3.write(esquerda);
      delay(tempoEntreServos);
    }else{
      if(status_botao5){
        esquerda -= fator;
        servo5_6.write(esquerda);
        delay(tempoEntreServos);
      }
    }
  }
  Serial.println("esquerda = " + String(esquerda));
  /*if(status_botao3){
    servo2_3.write(esquerda);
  }
  if(status_botao5){
    servo5_6.write(esquerda);
  }*/

  /*if(status_botao1 || status_botao3 || status_botao5){
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
  }

  if(status_botao2){
    direita += fator;
    if(status_botao4){
      direita += fator;
      if(status_botao6){
        direita += fator;
        servo5_6.write(direita);
        delay(tempoEntreServos);
      }
      servo1_4.write(direita);
      delay(tempoEntreServos);
    }else{
      if(status_botao6){
        direita += fator;
        servo5_6.write(direita);
        delay(tempoEntreServos);
      }
    }
    servo2_3.write(direita);
    delay(tempoEntreServos);
  }else{
    if(status_botao4){
      direita += fator;
      if(status_botao6){
        direita += fator;
        servo5_6.write(direita);
        delay(tempoEntreServos);
      }
      servo1_4.write(direita);
      delay(tempoEntreServos);
    }else{
      if(status_botao6){
        direita += fator;
        servo5_6.write(direita);
        delay(tempoEntreServos);
      }
    }
  }
  
  /*if(status_botao2){
    servo2_3.write(direita);
  }
  if(status_botao4){
    servo1_4.write(direita);
  }
  if(status_botao6){
    servo5_6.write(direita);
  }*/

  /*if(status_botao2 || status_botao4 || status_botao6){
    delay(tempoEntreGiros);
    servo1_4.write(centro);
    servo2_3.write(centro);
    servo5_6.write(centro);
    delay(tempoRetornarCentro);
  }
  Serial.println("direita = " + String(direita));
  posicionarProximoCaractere();
}*/


void marcarPontos(int status_botao1, int status_botao2, int status_botao3, int status_botao4, int status_botao5, int status_botao6){
  Serial.println("Método marcarPontos");
  int tempoEntreServos = 150;
  int tempoEntreGiros = 00;
  int tempoRetornarCentro = 150;
  int diferenca_centro = 45;
  if(status_botao1){
    esquerda = centro_servo1_4 - diferenca_centro;
    servo1_4.write(esquerda);
    delay(tempoEntreServos);
  }
  if(status_botao3){
    esquerda = centro_servo2_3 - diferenca_centro;
    servo2_3.write(esquerda);
    delay(tempoEntreServos);
  }
  if(status_botao5){
    esquerda = centro_servo5_6 - diferenca_centro;
    servo5_6.write(esquerda);
    delay(tempoEntreServos);
  }

  if(status_botao1 || status_botao3 || status_botao5){
    delay(tempoEntreGiros);
    servo1_4.write(centro_servo1_4);
    servo2_3.write(centro_servo2_3);
    servo5_6.write(centro_servo5_6);
    delay(tempoRetornarCentro);
  }
  
  if(status_botao2){
    esquerda = centro_servo2_3 + diferenca_centro;
    servo2_3.write(direita);
    delay(tempoEntreServos);
  }
  if(status_botao4){
    esquerda = centro_servo1_4 + diferenca_centro;
    servo1_4.write(direita);
    delay(tempoEntreServos);
  }
  if(status_botao6){
    esquerda = centro_servo5_6 + diferenca_centro;
    servo5_6.write(direita);
    delay(tempoEntreServos);
  }

  if(status_botao2 || status_botao4 || status_botao6){
    delay(tempoEntreGiros);
    servo1_4.write(centro_servo1_4);
    servo2_3.write(centro_servo2_3);
    servo5_6.write(centro_servo5_6);
    delay(tempoRetornarCentro);
  }
  posicionarProximoCaractere();
}

/*void girarServoMotor(String lado, Servo &servo){
  Serial.println("Método girarServoMotor");
  int valor = centro;
  Serial.println("lado = "+lado+", alteração = "+alteracaoLado);
  if (lado == "esquerdo"){
    Serial.println("entrou no if");
    alteracaoLado = -alteracaoLado;
  }
  for (int i = 0; i < quantAletaracoesLado; i++){
    valor = valor + alteracaoLado;
    servo.write(valor);
    //delay(tempoEntreGiros);
  }
}*/

void posicionarProximoCaractere(){
  Serial.println("Método posicionarProximoCaractere");
  countCaracteres++;
  Serial.println("Quant Caractere = " + String(countCaracteres));
  /*if (countCaracteres >= maxCaracteresLinha - 2){
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }*/
  if (countCaracteres >= maxCaracteresLinha - 1){
    //delay(100);
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }
  if (countCaracteres >= maxCaracteresLinha){
    delay(100);
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
    //posicionarProximaLinha(countCaracteres);
    posicionaProximaLinha();
    countCaracteres = 0;
    return;
  }
  motorX.step(deslocamentoCaractere, BACKWARD, metodoMotorPasso);
  motorX.release();
  //Serial.println("Entrou para alimentar o motor X");
}

void retornarPosicaoCarectereAnterior(){
  Serial.println("Método retornarPosicaoCarectereAnterior");
  Serial.println("Quant Caractere = " + String(countCaracteres));
  if (countCaracteres <= 2){
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }
  if (countCaracteres <= 1){
    delay(100);
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
  }
  if (countCaracteres <= 0){
    delay(100);
    // Aciona o buzzer na frequência relativa ao Ré em Hz   
    tone(buzzer,293);             
    delay(200);    
    noTone(buzzer);
    countCaracteres = 0;
    return;
  }

  if (countCaracteres > 0){
    motorX.step(deslocamentoCaractere, FORWARD, metodoMotorPasso);
    motorX.release();
    countCaracteres--;
    Serial.println("Quant Caractere 2 = " + String(countCaracteres));
  } 
}

void apagarCaractere(){
  Serial.println("Método apagarCaractere");
  tone(buzzer,293);             
  //delay(200);    
  //noTone(buzzer);
  motorY.step(deslocamentoPapelApagarCaractere, BACKWARD, metodoMotorPasso);
  motorY.release();
  noTone(buzzer);
  //apaga
  for (int i = 0; i < 5; i++){
    Serial.println("Entrou no for");
    digitalWrite(solenoide, HIGH);
    delay(150); 
    digitalWrite(solenoide, LOW);
    delay(200); 
  }
  tone(buzzer,293); 
  motorY.step(deslocamentoPapelApagarCaractere, FORWARD, metodoMotorPasso);
  motorY.release();
  noTone(buzzer);
}

void posicionaProximaLinha(){
  Serial.println("Método posicionaProximaLinha");
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
  Serial.println("Método posicionarProximaLinha");
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
  Serial.println("Método posicionaInicioLinha");
  motorX.setSpeed(velocidadeMovimentacao);
  while(!digitalRead(fimDeCurso)){
    motorX.step(1, FORWARD, metodoMotorPasso);
  }
  motorX.release();
  motorX.setSpeed(velocidadeEscrita);
}

void voltarInicioLinha(int quantCarecteresDeslocamento){
  Serial.println("Método voltarInicioLinha");
  int quantPassosRetornar = quantCarecteresDeslocamento * deslocamentoCaractere;
  motorX.setSpeed(velocidadeMovimentacao);
  motorX.step(quantPassosRetornar, FORWARD, metodoMotorPasso);
  motorX.release();
  motorX.setSpeed(velocidadeEscrita);
}

 void liberarPagina(){
  Serial.println("Método liberarPagina");
  int deslocamentoLiberarPagina = deslocamentoLinha * 3; //quantidade de linhas para o final da página
  motorY.setSpeed(velocidadeMovimentacao);
  motorY.step(deslocamentoLiberarPagina, BACKWARD, metodoMotorPasso);
  motorY.release();
  motorY.setSpeed(velocidadeEscrita);
 }

void puxarNovaPagina(){ //acionado pelo botão de posicionar nova página - Futuramente pode ser controlado por um sensor que identifique o término do papel e início de outro
  Serial.println("Método puxarNovaPagina");
  int deslocamentoPosicionarNovaPagina = deslocamentoLinha * 4; //quantidade de linhas para o final da página
  motorY.setSpeed(velocidadeMovimentacao);
  motorY.step(deslocamentoPosicionarNovaPagina, BACKWARD, metodoMotorPasso);
  motorY.release();
  motorY.setSpeed(velocidadeEscrita);
}

void resetarStatus(){
  Serial.println("Método resetarStatus");
    status_botao1 = 0;
    status_botao2 = 0;
    status_botao3 = 0;
    status_botao4 = 0;
    status_botao5 = 0;
    status_botao6 = 0;
    status_botaoEspaco = 0;
    /*if (alteracaoLado<0){
      alteracaoLado = -alteracaoLado;
    }*/
}
