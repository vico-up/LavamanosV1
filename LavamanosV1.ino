/*d
//Codigo lavamanos automatico v1, usando protothreads 
*/


#include <protothreads.h>

const int sensorJabon = 4;
const int ledOffJabon = 5;    //color rojo led apagado jabon
const int ledOnJabon = 6;   //color verde led encendido jabon
const int dispensador = 7; 

bool estadoSensorJ = false;    //guarda el estado del sensor jabon

const int sensorAgua = 8;
const int ledOffAgua = 9;    //color rojo led apagado jabon
const int ledOnAgua = 10;   //color verde led encendido jabon
const int bomba = 11; 

const int botonJabonn = 2;
bool estadoBoton = false;

const int botonTiempito = 13; //boton para determinar tiempo del dispensador
bool estadoBotonTiempito = false;
bool UltestadoBotonTiempito = true; //ultimo estado del boton tiempito
const int buzzer =12;

int contador = 0;   //

int tiempito = 0;
int tiempito1 = 2000;
int tiempito2 = 5000;
int tiempito3 = 10000;

pt ptBlink;     //ptblick es para controlar el teimpo del dispensdor de jabon
int blinkThread(struct pt* pt) 
{
  PT_BEGIN(pt);

  // Loop forever
  for(;;) 
  {
	  if (estadoSensorJ == true) 
    {
		digitalWrite(ledOffJabon, HIGH);
    digitalWrite(ledOnJabon, LOW);
    digitalWrite(dispensador, HIGH);   // Encender dispensador (HIGH is the voltage level)
    PT_SLEEP(pt, tiempito);                 //tiempo de activacion
    digitalWrite(ledOnJabon, HIGH);
    digitalWrite(ledOffJabon, LOW);
		digitalWrite(dispensador, LOW);    // apgar dispensador by making the voltage LOW
		PT_SLEEP(pt, 5000);               //tiempo para evitar desperdicio de jabon
	  } 
    
    else 
    {
		digitalWrite(dispensador, LOW);    // mantener apagado sipensador si no hay activacion
    digitalWrite(ledOffJabon, LOW);    //mantiene encendido led rojo de jabon
    digitalWrite(ledOnJabon, HIGH);
		PT_YIELD(pt);
	  }
  }

  PT_END(pt);
}

pt ptButton;      //ptButton controla el estado del sensor de jabon
int buttonThread(struct pt* pt) 
{
  PT_BEGIN(pt);

  // Loop forever
  for(;;) 
  {
	// read the state of the pushbutton value:
  	estadoSensorJ = digitalRead(sensorJabon);
	  PT_YIELD(pt);
  }

  PT_END(pt);
}

void setup()
{
  Serial.begin(9600);
  tiempito = tiempito1; //inicia por default en modo dispensador(poco jabon para dispensar)
  PT_INIT(&ptBlink);    // INICIALIZAR protothreads
  PT_INIT(&ptButton);

  pinMode(botonJabonn, INPUT);
  pinMode(sensorJabon, INPUT);
  pinMode(dispensador, OUTPUT);
  pinMode(ledOffJabon, OUTPUT);
  pinMode(ledOnJabon, OUTPUT);

  pinMode(sensorAgua, INPUT);
  pinMode(bomba, OUTPUT);
  pinMode(ledOffAgua, OUTPUT);
  pinMode(ledOnAgua, OUTPUT);

  pinMode(botonTiempito, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  digitalWrite(ledOnJabon, HIGH);
  digitalWrite(ledOffJabon, LOW);

  digitalWrite(ledOnAgua, LOW);
  digitalWrite(ledOffAgua, HIGH);





  
}

void loop()
{
  
  PT_SCHEDULE(blinkThread(&ptBlink));   //llamar a funciones de contro y encendido de dispensador
  PT_SCHEDULE(buttonThread(&ptButton));
  activarAgua();

  while (digitalRead(botonJabonn) == true)  //modo continuo de jabon
  {
    jabonContinuo();
  }

  estadoBotonTiempito = digitalRead(botonTiempito);
  if (estadoBotonTiempito != UltestadoBotonTiempito)
  {
    if (estadoBotonTiempito == true)
    {
      contador++;
      delay(600);
      Serial.println(contador);
    }
  }
  UltestadoBotonTiempito = estadoBotonTiempito;

  if (contador == 1)
      {
        modo1();
      }
      else if (contador == 3)
      {
        modo2();
      }
      else if (contador == 5)
      {
        modo3();
      }
}

void activarAgua()
{
    bool estadoSensorA = false;
    estadoSensorA = digitalRead(sensorAgua);
    if (estadoSensorA == true)
    {
      digitalWrite(ledOffAgua, LOW);
      digitalWrite(ledOnAgua, HIGH);
      digitalWrite(bomba, HIGH);
    }

    else 
    {
      digitalWrite(bomba, LOW);
      digitalWrite(ledOnAgua, LOW);
      digitalWrite(ledOffAgua, HIGH);

    }
}

void jabonContinuo()
{
  digitalWrite(ledOffJabon, HIGH);
  digitalWrite(ledOnJabon, LOW);
  digitalWrite(dispensador, HIGH);
}

void jabonContinuoOff()
{
  digitalWrite(dispensador, LOW);    // mantener apagado sipensador si no hay activacion
  digitalWrite(ledOnJabon, HIGH);
  digitalWrite(ledOffJabon, LOW);
}

void modo2()
{
    tiempito = tiempito2;
    digitalWrite(ledOffJabon, HIGH);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);
    delay(250);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);

    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);

    contador = contador + 1 ;
  
}

void modo3()
{
  
    tiempito = tiempito3;
    digitalWrite(ledOffJabon, HIGH);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);
    delay(250);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);
    delay(250);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);

    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);

    contador = 0;
  
}

void modo1()
{
  
    tiempito = tiempito1;
    digitalWrite(ledOffJabon, HIGH);
    digitalWrite(ledOnJabon, LOW);
    delay(500);
    digitalWrite(ledOnJabon, HIGH);
    
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);

    contador = contador + 1;
}