/*Test_Segainvex_scpi_Serial.ino
 Ejemplo de uso de la librería segainvex_scpi_Serial para 
 comunicar un PC con Arduino a través del puerto serie "Serial" 
*/
#include <Arduino.h>
#include <segainvex_scpi_Serial.h>
/*Prototipos de funciones*/
void fs1(void);
void fs2(void);
void fs3(void);
void fs4(void);
/*
 submenú al que llamamos por ejemplo "SUBMENU1". Contiene dos 
 comandos, a los que decidimos llamar,COMANDO11 con nombre
 abreviado C11 y COMANDO12 con nombre abreviado C12. El 
 primero hará que se ejecute la función "void fs3(void)"
 y el segundo la función "void fs4(void)" que definiremos
 como cualquier otra función.
*/
 tipoNivel SUBMENU1[] = //Array de estructuras tipo Nivel
{
	SCPI_COMANDO(COMANDO11,C11,fs3)//Comando que ejecuta la función fs3()
	SCPI_COMANDO(COMANDO12,C12,fs4)//Comando que ejecuta la función fs4()
	// TO DO Añadir aquí más comandos o submenús
};
/*
  Ahora definimos “NivelDos” obligatorio en todas las 
  aplicaciones
*/
 tipoNivel NivelDos[] = //Array de estructuras tipo Nivel
{
//Submenú con comandos declarado más arriba
SCPI_SUBMENU(SUBMENU1,SM1 )	
//Comandos definidos por el usuario
	SCPI_COMANDO(COMANDO1,C1,fs1) //Comando que ejecuta la función fs1()
	SCPI_COMANDO(COMANDO2,C2,fs2) //Comando que ejecuta la función fs2()
	//Comandos que ejecutan funciones definidas en la librería segainvex_scpi_serial
	// TO DO Añadir aquí comandos o submenús
	//Comandos de funciones definidas en Segainvex_scpi_Serial
	SCPI_COMANDO(ERROR,ERR,scpi_error)// Envía el ultimo error
	SCPI_COMANDO(*IDN,*IDN,scpi_idn)// Identifica el instrumento
	SCPI_COMANDO(*OPC,*OPC,scpi_opc)// Devuelve un 1 al PC
	SCPI_COMANDO(*CLS,*CLS,scpi_cls)// Borra la pila de errores
};

/*
 Vemos que “NivelDos” contiene la declaración del submenú 
 SUBMENU1 definido anteriormente. Además contiene dos comandos
 a los que hemos decidido llamar COMANDO1 y COMANDO2 con 
 nombre abreviado C1 y C2 que ejecutan las funciones que
 definiremos más adelante “void fs1(void)” y “void fs2(void)”;
 También hay cuatro comandos más ERROR, *IDN, OPC, y *CLS que
 ejecutan las funciones ·”fs240”,”fs243”,”fs248” y ”fs255” 
 que ya están definidas en la librería y que se explicarán 
 más adelante. 
*/
/*Por último declaramos el nivel raíz:*/
SCPI_NIVEL_RAIZ// Macro que hace la declaración obligatoria del nivel Raiz
//Solo nos falta definir la lista de errores:
/*
CodigoError está declarado en scpi.h, por lo que es global. Es obligatorio
definirlo aquí. También es obligatorio incluir los primeros 6 errores y
denominarlos como se ve a continuación.
Cada error que quiera registrar el usuario ha de incluirlo desde el 7 en adelante.
*/
tipoCodigoError CodigoError=
{
  // Errores del sistema SCPI 0...6
  " ",						// ERROR N. 0
  "1 Caracter no valido",                // ERROR N. 1
  "2 Comando desconocido",               // ERROR N. 2
  "3 Cadena demasiado larga",            // ERROR N. 3
  "4 Parametro inexistente",             // ERROR N. 4
  "5 Formato de parametro no valido",    // ERROR N. 5
  "6 Parametro fuera de rango",          // ERROR N. 6
  // Errores personalizados por el usuario 
  "7 El dato no esta listo",		// ERROR N. 7
};	

//Ahora el código habitual de Arduino:

void setup() 
{
//Función para dar un nombre a nuestro sistema
scpiIDN("Prueba de segainvex_scpi_serial para Arduino V1.0");
// Abre el puerto serie
Serial.begin(115200); 
}
void loop()
{
  // Si recibe algo por el puerto serie lo procesa con segainvex_scpi_serial 
  if (Serial.available()){scpi(&Serial);}
/*
TO DO Poner aquí el código de usuario
*/
}// loop() 

//Ahora solo nos queda definir las funciones de nuestro sistema:

void fs1(void)
{
 ESCRIBE_PUERTO_SERIE
 ("Se ha recibido el COMANDO1 y se ha ejecutado la funcion fs1");	
}
void fs2(void)
{
 ESCRIBE_PUERTO_SERIE
 ("Se ha recibido el COMANDO2 y se ha ejecutado la funcion fs2");	
}
/****************************************************************
	Comando SUBMENU1:COMANDO11 ó SM1:C11
	Ejecuta la función void fs3(void);
	Cambia el valor de la variable tipo double "Variable1"
*****************************************************************/
void fs3(void)
{
	static double Variable1=1.1;
	int Resultado;
	Resultado=cambia_variable_double_del_sistema(&Variable1,10.0,0.0);
	switch (Resultado)
	{
		case 0:
			ESCRIBE_PUERTO_SERIE("No Se cambio el valor de la Variable1");
			errorscpi(7);
		break;
		case 1:
			ESCRIBE_PUERTO_SERIE("Se cambio el valor de la Variable1");
		break;
		case 2:
			ESCRIBE_PUERTO_SERIE("Se envio al PC el valor de la Variable1");
		break;
	}
}
/*****************************************************************
	Comando SUBMENU1:COMANDO11 ó SM1:C12
	Ejecuta la función void fs4(void);
	Cambia el valor de la variable discreta tipo int "Variable2"
*****************************************************************/
void fs4(void)
{
	int Resultado;
	static int Variable2=100;
	int ValoresVariable2[]={1,10,100};
	Resultado=cambia_variable_int_discreta_del_sistema(&Variable2,
	ValoresVariable2,sizeof(ValoresVariable2));
	switch (Resultado)
	{
		case 0:
			ESCRIBE_PUERTO_SERIE("No Se cambio el valor de la Variable2");
		errorscpi(7);
		break;
		case 1:
			ESCRIBE_PUERTO_SERIE("Se cambio el valor de la Variable2");
		break;
		case 2:
			ESCRIBE_PUERTO_SERIE("Se envio al PC el valor de la Variable2");
		break;
	}	
}
/*
  Prueba a enviar por el puerto serie los comandos (terminados con \r)
  sm1:c12?
  sm1:c12 10
  sm1:c12?
  c1
  c2
  err?
  *idn
  err?:*idn
*/
/*********************/ /*FIN********************************************/
 
