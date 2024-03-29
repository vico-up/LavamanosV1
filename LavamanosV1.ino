/*d
//Codigo lavamanos automatico v1, usando protothreads 
*/

/*
Tareas
ajustar tiempo para dispensar el jabon definitivamente

*/



#include <protothreads.h>
#include <EEPROM.h>

const int sensorJabon = 4;
const int ledOffJabon = 5;    //color rojo led apagado jabon
const int ledOnJabon = 6;   //color verde led encendido jabon
const int dispensador = 7;  //salida para activar el rele del dispensador

bool estadoSensorJ = true;    //guarda el estado del sensor jabon(True si el sensor es NPN(logica negativa))

const int sensorAgua = 8;
const int ledOffAgua = 9;    //color rojo led apagado jabon
const int ledOnAgua = 10;   //color verde led encendido jabon
const int bombaAgua = 11;     //salida para activar el rele de la (Bomba, electrovalvula)

bool estadoSensorAgua = true;  //guarda el estado del sensor agua(True si el sensor es NPN(logica negativa))

const int botonJabonn = 2;    //modo continuo jabon para dispensar nueva botella
bool estadoBoton = false;  //Creo que no se esta usando esta varialble   

const int botonTiempito = 3; //boton para determinar tiempo del dispensador
bool estadoBotonTiempito = false;
bool UltestadoBotonTiempito = true; //ultimo estado del boton tiempito
const int buzzer = 12;

int contador = 0;   //variable se usa en el tiempo de dispensado de jabon
int ultimoContador;

int tiempito = 0;
//variables para guardar el tiempo de dispensado de jabon
int tiempito1 = 1100;
int tiempito2 = 2100; //pareca que este va a ser maximo
//int tiempito3 = 3200;
int ultimoTiempito;

pt ptActivarAgua;
int activarAguaThread(struct pt* pt)
{
    PT_BEGIN(pt);

    for(;;)
    {
        if(estadoSensorAgua == false) //compara con logica negativa por el sensor es NPN(se activa con logica negativa)
        {
            digitalWrite(ledOffAgua, LOW);
            digitalWrite(ledOnAgua, HIGH);
            digitalWrite(bombaAgua, HIGH);
            PT_SLEEP(pt, 5000);   //tiempo de espera para evitar falso del sensor de agua y no apague y prenda
        }

        else
        {
            digitalWrite(ledOffAgua, HIGH);
            digitalWrite(ledOnAgua, LOW);
            digitalWrite(bombaAgua, LOW);
            PT_YIELD(pt);
        }
    }
    PT_END(pt);
}

pt ptSensorAgua;
int sensorAguaThread(struct pt* pt) 
{
  PT_BEGIN(pt);

  for(;;) 
  {	
  	estadoSensorAgua = digitalRead(sensorAgua);
	  PT_YIELD(pt);
  }

  PT_END(pt);
}



pt ptBlink;     //ptblick es para controlar el teimpo del dispensdor de jabon
int blinkThread(struct pt* pt) 
{
  PT_BEGIN(pt);

  for(;;) 
  {
	  if (estadoSensorJ == false) 
    {
		digitalWrite(ledOffJabon, LOW);
    digitalWrite(ledOnJabon, HIGH);
    digitalWrite(dispensador, HIGH);   // Encender dispensador (HIGH is the voltage level)
    PT_SLEEP(pt, tiempito);                 //tiempo de activacion
    digitalWrite(ledOnJabon, LOW);
    digitalWrite(ledOffJabon, HIGH);
		digitalWrite(dispensador, LOW);    // apgar dispensador by making the voltage LOW
		PT_SLEEP(pt, 5000);               //tiempo para evitar desperdicio de jabon
	  } 
    
    else 
    {
		digitalWrite(dispensador, LOW);    // mantener apagado sipensador si no hay activacion
    digitalWrite(ledOffJabon, HIGH);    //mantiene encendido led rojo de jabon
    digitalWrite(ledOnJabon, LOW);
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
  tiempito = EEPROM.get(10, ultimoTiempito); //retorna el ultimo dato de contadr y tiempito
  contador = EEPROM.get(0, ultimoContador);
  
  PT_INIT(&ptActivarAgua);
  PT_INIT(&ptSensorAgua);

  PT_INIT(&ptBlink);    // INICIALIZAR protothreads
  PT_INIT(&ptButton);

  pinMode(botonJabonn, INPUT);
  pinMode(sensorJabon, INPUT);
  pinMode(dispensador, OUTPUT);
  pinMode(ledOffJabon, OUTPUT);
  pinMode(ledOnJabon, OUTPUT);

  pinMode(sensorAgua, INPUT);
  pinMode(bombaAgua, OUTPUT);
  pinMode(ledOffAgua, OUTPUT);
  pinMode(ledOnAgua, OUTPUT);

  pinMode(botonTiempito, INPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(ledOnJabon, LOW);
  digitalWrite(ledOffJabon, HIGH);

  digitalWrite(ledOnAgua, LOW);
  digitalWrite(ledOffAgua, HIGH);
  
  /*Serial.println("Ultimo tiempo es: "); 
  Serial.println(EEPROM.get(10, ultimoTiempito));
  delay(2000);  
  Serial.println("Ultimo contador es: ");
  Serial.println(EEPROM.get(0, ultimoContador));*/
  
}

void loop()
{
  PT_SCHEDULE(activarAguaThread(&ptActivarAgua));
  PT_SCHEDULE(sensorAguaThread(&ptSensorAgua));

  PT_SCHEDULE(blinkThread(&ptBlink));   //llamar a funciones de contro y encendido de dispensador
  PT_SCHEDULE(buttonThread(&ptButton));

  //llma a la funcion activar agua sin prototreads borrardespues de realizar pruebas
  //activarAgua();

  while (digitalRead(botonJabonn) == true)  //modo continuo de jabon
  {
    jabonContinuo();
  }

  estadoBotonTiempito = digitalRead(botonTiempito); //Lee pulsador de cantidad de dispensado de jabon
  if (estadoBotonTiempito != UltestadoBotonTiempito)
  {
    if (estadoBotonTiempito == true)
    {
      llamarBuzzer();
      contador++;
      delay(600);
      Serial.print("El contador es: ");
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
      /*
      else if (contador == 5)
      {
        modo3();
      }*/
}
/*Borrar despues de realizar pruebas
void activarAgua()
{
    bool estadoSensorA = false;
    estadoSensorA = digitalRead(sensorAgua);
    if (estadoSensorA == true)
    {
      digitalWrite(ledOffAgua, LOW);
      digitalWrite(ledOnAgua, HIGH);
      digitalWrite(bomba, LOW);   //logica invertida por sensor NPN(activacion con nivel logico bajo)
      
    }

    else 
    {
      digitalWrite(bomba, HIGH);
      digitalWrite(ledOnAgua, LOW);
      digitalWrite(ledOffAgua, HIGH);

    }
    delay(500);     //delay para estabilidad contra falsa deteccion
}
*/
void jabonContinuo()
{
  digitalWrite(ledOffJabon, LOW);
  digitalWrite(ledOnJabon, HIGH);
  digitalWrite(dispensador, HIGH);
}

void jabonContinuoOff()
{
  digitalWrite(dispensador, HIGH);    // mantener apagado sipensador si no hay activacion
  digitalWrite(ledOnJabon, LOW);
  digitalWrite(ledOffJabon, LOW);
}

void llamarBuzzer()
{
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
}

void modo2()
{
    tiempito = tiempito2;
    ultimoTiempito = tiempito;
    EEPROM.put(10, ultimoTiempito);

    digitalWrite(ledOffJabon, LOW);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);
    delay(250);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);

    
    contador = 0;
    ultimoContador = contador;
    EEPROM.put(0, ultimoContador);
    /*
    Serial.println("Ultimo tiempo es: "); 
    Serial.println(EEPROM.get(10, ultimoTiempito));
    
    Serial.println("Ultimo contador es: ");
    Serial.println(EEPROM.get(0, ultimoContador));
    */
}
/*
void modo3()
{
  
    tiempito = tiempito3;
    ultimoTiempito = tiempito;
    EEPROM.put(10, ultimoTiempito);

    digitalWrite(ledOffJabon, LOW);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);
    delay(250);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);
    delay(250);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);

    contador = 0;
    ultimoContador = contador;
    EEPROM.put(0, ultimoContador);
    
    Serial.println("Ultimo tiempo es: "); 
    Serial.println(EEPROM.get(10, ultimoTiempito));
    
    Serial.println("Ultimo contador es: ");
    Serial.println(EEPROM.get(0, ultimoContador));
    
}
*/

void modo1()
{
  
    tiempito = tiempito1;
    ultimoTiempito = tiempito;
    EEPROM.put(10, ultimoTiempito);

    digitalWrite(ledOffJabon, LOW);
    digitalWrite(ledOnJabon, HIGH);
    delay(500);
    digitalWrite(ledOnJabon, LOW);
    
    contador = contador + 1;
    ultimoContador = contador;
    EEPROM.put(0, ultimoContador);
    /*
    Serial.println("Ultimo tiempo es: "); 
    Serial.println(EEPROM.get(10, ultimoTiempito));
    
    Serial.println("Ultimo contador es: ");
    Serial.println(EEPROM.get(0, ultimoContador));
    */
}